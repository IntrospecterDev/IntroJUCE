/*
  ==============================================================================

   This file is part of the JUCE 8 technical preview.
   Copyright (c) Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce::midi_ci
{

class ProfileHost::Visitor : public detail::MessageTypeUtils::MessageVisitor
{
public:
    Visitor (ProfileHost* h, ResponderOutput* o, bool* b)
        : host (h), output (o), handled (b) {}

    void visit (const Message::ProfileInquiry& body) const override { visitImpl (body); }
    void visit (const Message::ProfileDetails& body) const override { visitImpl (body); }
    void visit (const Message::ProfileOn& body)      const override { visitImpl (body); }
    void visit (const Message::ProfileOff& body)     const override { visitImpl (body); }
    using MessageVisitor::visit;

    static auto getNumChannels (Message::Header header, Message::ProfileOn p)
    {
        return (uint8_t) header.version >= 2 ? p.numChannels : 1;
    }

    static auto getNumChannels (Message::Header, Message::ProfileOff)  { return 0; }

private:
    template <typename Body>
    void visitImpl (const Body& body) const { *handled = messageReceived (body); }

    bool messageReceived (const Message::ProfileInquiry&) const
    {
        host->isResponder = true;

        if ((uint8_t) output->getIncomingHeader().deviceID < 16
            || output->getIncomingHeader().deviceID == ChannelInGroup::wholeGroup)
        {
            if (const auto* state = host->getProfileStates().groupStates[output->getIncomingGroup()].getStateForDestination (output->getIncomingHeader().deviceID))
            {
                const auto active   = state->getActive();
                const auto inactive = state->getInactive();
                detail::MessageTypeUtils::send (*output, Message::ProfileInquiryResponse { active, inactive });
            }
        }
        else if (output->getIncomingHeader().deviceID == ChannelInGroup::wholeBlock)
        {
            auto header = output->getReplyHeader (detail::MessageMeta::Meta<Message::ProfileInquiryResponse>::subID2);

            const auto sendIfNonEmpty = [&] (const auto group, const auto& state)
            {
                if (! state.empty())
                {
                    const auto active   = state.getActive();
                    const auto inactive = state.getInactive();
                    detail::MessageTypeUtils::send (*output, (uint8_t) group, header, Message::ProfileInquiryResponse { active, inactive });
                }
            };

            for (auto groupNum = 0; groupNum < host->functionBlock.numGroups; ++groupNum)
            {
                const auto group = host->functionBlock.firstGroup + groupNum;
                const auto& groupState = host->getProfileStates().groupStates[(size_t) group];

                for (size_t channel = 0; channel < groupState.channelStates.size(); ++channel)
                {
                    header.deviceID = ChannelInGroup (channel);
                    sendIfNonEmpty (group, groupState.channelStates[channel]);
                }
            }

            header.deviceID = ChannelInGroup::wholeGroup;

            for (auto i = 0; i < host->functionBlock.numGroups; ++i)
            {
                const auto group = host->functionBlock.firstGroup + i;
                const auto& groupState = host->getProfileStates().groupStates[(size_t) group];
                sendIfNonEmpty (group, groupState.groupState);
            }

            // Always send the block response to indicate that no further replies will follow
            header.deviceID = ChannelInGroup::wholeBlock;
            const auto state = host->getProfileStates().blockState;
            const auto active   = state.getActive();
            const auto inactive = state.getInactive();
            detail::MessageTypeUtils::send (*output, output->getIncomingGroup(), header, Message::ProfileInquiryResponse { active, inactive });
        }

        return true;
    }

    bool messageReceived (const Message::ProfileDetails& body) const
    {
        if (body.target == std::byte{})
        {
            const auto address = ChannelAddress{}.withGroup (output->getIncomingGroup())
                                                 .withChannel (output->getIncomingHeader().deviceID);
            const ProfileAtAddress profileAtAddress { body.profile, address };
            const auto state = host->getState (profileAtAddress);
            std::vector<std::byte> extraData;
            detail::Marshalling::Writer { extraData } (state.active, state.supported);
            detail::MessageTypeUtils::send (*output, Message::ProfileDetailsResponse { body.profile, body.target, extraData });
        }
        else
        {
            detail::MessageTypeUtils::sendNAK (*output, std::byte { 0x04 });
        }

        return true;
    }

    template <typename Body>
    bool profileEnablementReceived (const Body& request) const
    {
        const auto destination = ChannelAddress{}.withGroup (output->getIncomingGroup())
                                                 .withChannel (output->getIncomingHeader().deviceID);
        if (auto* state = host->states.getStateForDestination (destination))
        {
            if (state->get (request.profile).isSupported())
            {
                const auto address = ChannelAddress{}.withGroup (output->getIncomingGroup())
                                                     .withChannel (output->getIncomingHeader().deviceID);
                const ProfileAtAddress profileAtAddress { request.profile, address };

                {
                    const ScopedValueSetter scope { host->currentEnablementMessage,
                                                    std::optional<ProfileAtAddress> (profileAtAddress) };
                    host->delegate.profileEnablementRequested (output->getIncomingHeader().source,
                                                               profileAtAddress,
                                                               getNumChannels (output->getIncomingHeader(), request),
                                                               std::is_same_v<Message::ProfileOn, Body>);
                }

                const auto currentState = host->getState (profileAtAddress);

                const auto sendResponse = [&] (auto response)
                {
                    const Message::Header header
                    {
                        profileAtAddress.address.getChannel(),
                        detail::MessageMeta::Meta<decltype (response)>::subID2,
                        detail::MessageMeta::implementationVersion,
                        output->getMuid(),
                        MUID::getBroadcast(),
                    };

                    detail::MessageTypeUtils::send (*output, profileAtAddress.address.getGroup(), header, response);
                };

                if (currentState.isActive())
                    sendResponse (Message::ProfileEnabledReport { profileAtAddress.profile, currentState.active });
                else
                    sendResponse (Message::ProfileDisabledReport { profileAtAddress.profile, 0 });

                host->isResponder = true;
                return true;
            }
        }

        detail::MessageTypeUtils::sendNAK (*output, {});
        return true;
    }

    bool messageReceived (const Message::ProfileOn& request) const
    {
        return profileEnablementReceived (request);
    }

    bool messageReceived (const Message::ProfileOff& request) const
    {
        return profileEnablementReceived (request);
    }

    ProfileHost* host = nullptr;
    ResponderOutput* output = nullptr;
    bool* handled = nullptr;
};

void ProfileHost::addProfile (ProfileAtAddress profileAtAddress, int maxNumChannels)
{
    auto* state = states.getStateForDestination (profileAtAddress.address);

    if (state == nullptr || state->get (profileAtAddress.profile).isSupported())
        return;

    // There are only 256 channels on a UMP endpoint, so requesting more probably doesn't make sense!
    jassert (maxNumChannels <= 256);

    state->set (profileAtAddress.profile, { (uint16_t) maxNumChannels, 0 });

    if (! isResponder || profileAtAddress == currentEnablementMessage)
        return;

    const Message::Header header
    {
        profileAtAddress.address.getChannel(),
        detail::MessageMeta::Meta<Message::ProfileAdded>::subID2,
        detail::MessageMeta::implementationVersion,
        output.getMuid(),
        MUID::getBroadcast(),
    };

    detail::MessageTypeUtils::send (output,
                                    profileAtAddress.address.getGroup(),
                                    header,
                                    Message::ProfileAdded { profileAtAddress.profile });
}

void ProfileHost::removeProfile (ProfileAtAddress profileAtAddress)
{
    auto* state = states.getStateForDestination (profileAtAddress.address);

    if (state == nullptr)
        return;

    disableProfile (profileAtAddress);

    if (! state->get (profileAtAddress.profile).isSupported())
        return;

    state->erase (profileAtAddress.profile);

    if (! isResponder || profileAtAddress == currentEnablementMessage)
        return;

    const Message::Header header
    {
        profileAtAddress.address.getChannel(),
        detail::MessageMeta::Meta<Message::ProfileRemoved>::subID2,
        detail::MessageMeta::implementationVersion,
        output.getMuid(),
        MUID::getBroadcast(),
    };

    detail::MessageTypeUtils::send (output,
                                    profileAtAddress.address.getGroup(),
                                    header,
                                    Message::ProfileRemoved { profileAtAddress.profile });
}

void ProfileHost::enableProfile (ProfileAtAddress profileAtAddress, int numChannels)
{
    auto* state = states.getStateForDestination (profileAtAddress.address);

    if (state == nullptr)
        return;

    const auto old = state->get (profileAtAddress.profile);

    if (! old.isSupported())
        return;

    // There are only 256 channels on a UMP endpoint, so requesting more probably doesn't make sense!
    jassert (numChannels <= 256);

    const auto enabledChannels = jmin (old.supported, (uint16_t) numChannels);
    state->set (profileAtAddress.profile, { old.supported, enabledChannels });

    if (! isResponder || profileAtAddress == currentEnablementMessage)
        return;

    const Message::Header header
    {
        profileAtAddress.address.getChannel(),
        detail::MessageMeta::Meta<Message::ProfileEnabledReport>::subID2,
        detail::MessageMeta::implementationVersion,
        output.getMuid(),
        MUID::getBroadcast(),
    };

    detail::MessageTypeUtils::send (output,
                                    profileAtAddress.address.getGroup(),
                                    header,
                                    Message::ProfileEnabledReport { profileAtAddress.profile, enabledChannels });
}

void ProfileHost::disableProfile (ProfileAtAddress profileAtAddress)
{
    auto* state = states.getStateForDestination (profileAtAddress.address);

    if (state == nullptr)
        return;

    const auto old = state->get (profileAtAddress.profile);

    if (! old.isActive())
        return;

    state->set (profileAtAddress.profile, { old.supported, 0 });

    if (! isResponder || profileAtAddress == currentEnablementMessage)
        return;

    const Message::Header header
    {
        profileAtAddress.address.getChannel(),
        detail::MessageMeta::Meta<Message::ProfileDisabledReport>::subID2,
        detail::MessageMeta::implementationVersion,
        output.getMuid(),
        MUID::getBroadcast(),
    };

    detail::MessageTypeUtils::send (output,
                                    profileAtAddress.address.getGroup(),
                                    header,
                                    Message::ProfileDisabledReport { profileAtAddress.profile, old.active });
}

bool ProfileHost::tryRespond (ResponderOutput& responderOutput, const Message::Parsed& message)
{
    bool result = false;
    detail::MessageTypeUtils::visit (message, Visitor { this, &responderOutput, &result });
    return result;
}

} // namespace juce::midi_ci
