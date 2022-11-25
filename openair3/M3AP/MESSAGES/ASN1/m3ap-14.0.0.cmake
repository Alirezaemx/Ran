set(M3AP_GRAMMAR ASN1/m3ap-14.0.0.asn)

set(m3ap_source
    ANY_aper.c
    ANY.c
    ANY_xer.c
    aper_decoder.c
    aper_encoder.c
    aper_opentype.c
    aper_support.c
    asn_application.c
    asn_bit_data.c
    asn_codecs_prim.c
    asn_codecs_prim_xer.c
    asn_internal.c
    asn_random_fill.c
    asn_SEQUENCE_OF.c
    asn_SET_OF.c
    ber_tlv_length.c
    ber_tlv_tag.c
    BIT_STRING.c
    BIT_STRING_print.c
    BIT_STRING_rfill.c
    BIT_STRING_xer.c
    constraints.c
    constr_CHOICE_aper.c
    constr_CHOICE.c
    constr_CHOICE_print.c
    constr_CHOICE_rfill.c
    constr_CHOICE_xer.c
    constr_SEQUENCE_aper.c
    constr_SEQUENCE.c
    constr_SEQUENCE_OF_aper.c
    constr_SEQUENCE_OF.c
    constr_SEQUENCE_OF_xer.c
    constr_SEQUENCE_print.c
    constr_SEQUENCE_rfill.c
    constr_SEQUENCE_xer.c
    constr_SET_OF_aper.c
    constr_SET_OF.c
    constr_SET_OF_print.c
    constr_SET_OF_rfill.c
    constr_SET_OF_xer.c
    constr_TYPE.c
    GraphicString.c
    INTEGER_aper.c
    INTEGER.c
    INTEGER_print.c
    INTEGER_rfill.c
    INTEGER_xer.c
    M3AP_Absolute-Time-ofMBMS-Data.c
    M3AP_AllocationAndRetentionPriority.c
    M3AP_BitRate.c
    M3AP_Cause.c
    M3AP_CauseMisc.c
    M3AP_CauseNAS.c
    M3AP_CauseProtocol.c
    M3AP_CauseRadioNetwork.c
    M3AP_CauseTransport.c
    M3AP_Criticality.c
    M3AP_CriticalityDiagnostics.c
    M3AP_CriticalityDiagnostics-IE-List.c
    M3AP_ECGI.c
    M3AP_ErrorIndication.c
    M3AP_EUTRANCellIdentifier.c
    M3AP_ExtendedMCE-ID.c
    M3AP_EXTERNAL.c
    M3AP_GBR-QosInformation.c
    M3AP_Global-MCE-ID.c
    M3AP_GTP-TEID.c
    M3AP_InitiatingMessage.c
    M3AP_IPAddress.c
    M3AP_M3AP-PDU.c
    M3AP_M3SetupFailure.c
    M3AP_M3SetupRequest.c
    M3AP_M3SetupResponse.c
    M3AP_MBMS-Cell-List.c
    M3AP_MBMS-E-RAB-QoS-Parameters.c
    M3AP_MBMSServiceArea1.c
    M3AP_MBMS-Service-Area.c
    M3AP_MBMSServiceAreaListItem.c
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionItem.c
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionListResAck.c
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionListRes.c
    M3AP_MBMS-Session-Duration.c
    M3AP_MBMS-Session-ID.c
    M3AP_MBMSSessionStartFailure.c
    M3AP_MBMSSessionStartRequest.c
    M3AP_MBMSSessionStartResponse.c
    M3AP_MBMSSessionStopRequest.c
    M3AP_MBMSSessionStopResponse.c
    M3AP_MBMSSessionUpdateFailure.c
    M3AP_MBMSSessionUpdateRequest.c
    M3AP_MBMSSessionUpdateResponse.c
    M3AP_MCEConfigurationUpdateAcknowledge.c
    M3AP_MCEConfigurationUpdate.c
    M3AP_MCEConfigurationUpdateFailure.c
    M3AP_MCE-ID.c
    M3AP_MCE-MBMS-M3AP-ID.c
    M3AP_MCEname.c
    M3AP_MinimumTimeToMBMSDataTransfer.c
    M3AP_MME-MBMS-M3AP-ID.c
    M3AP_PLMN-Identity.c
    M3AP_Pre-emptionCapability.c
    M3AP_Pre-emptionVulnerability.c
    M3AP_Presence.c
    M3AP_PriorityLevel.c
    M3AP_PrivateIE-Container.c
    M3AP_PrivateIE-Field.c
    M3AP_PrivateIE-ID.c
    M3AP_PrivateMessage.c
    M3AP_ProcedureCode.c
    M3AP_ProtocolExtensionContainer.c
    M3AP_ProtocolExtensionField.c
    M3AP_ProtocolIE-Container.c
    M3AP_ProtocolIE-ContainerList.c
    M3AP_ProtocolIE-ContainerPair.c
    M3AP_ProtocolIE-ContainerPairList.c
    M3AP_ProtocolIE-Field.c
    M3AP_ProtocolIE-FieldPair.c
    M3AP_ProtocolIE-ID.c
    M3AP_ProtocolIE-Single-Container.c
    M3AP_QCI.c
    M3AP_Reestablishment.c
    M3AP_ResetAcknowledge.c
    M3AP_ResetAll.c
    M3AP_Reset.c
    M3AP_ResetType.c
    M3AP_SuccessfulOutcome.c
    M3AP_TimeToWait.c
    M3AP_TMGI.c
    M3AP_TNL-Information.c
    M3AP_TriggeringMessage.c
    M3AP_TypeOfError.c
    M3AP_UnsuccessfulOutcome.c
    NativeEnumerated_aper.c
    NativeEnumerated.c
    NativeEnumerated_xer.c
    NativeInteger_aper.c
    NativeInteger.c
    NativeInteger_print.c
    NativeInteger_rfill.c
    NativeInteger_xer.c
    ObjectDescriptor.c
    OBJECT_IDENTIFIER.c
    OBJECT_IDENTIFIER_print.c
    OBJECT_IDENTIFIER_rfill.c
    OBJECT_IDENTIFIER_xer.c
    OCTET_STRING_aper.c
    OCTET_STRING.c
    OCTET_STRING_print.c
    OCTET_STRING_rfill.c
    OCTET_STRING_xer.c
    OPEN_TYPE_aper.c
    OPEN_TYPE.c
    OPEN_TYPE_xer.c
    per_decoder.c
    per_encoder.c
    per_opentype.c
    per_support.c
    PrintableString.c
    xer_decoder.c
    xer_encoder.c
    xer_support.c
)

set(m3ap_headers
    ANY.h
    aper_decoder.h
    aper_encoder.h
    aper_opentype.h
    aper_support.h
    asn_application.h
    asn_bit_data.h
    asn_codecs.h
    asn_codecs_prim.h
    asn_config.h
    asn_internal.h
    asn_ioc.h
    asn_random_fill.h
    asn_SEQUENCE_OF.h
    asn_SET_OF.h
    asn_system.h
    ber_tlv_length.h
    ber_tlv_tag.h
    BIT_STRING.h
    constraints.h
    constr_CHOICE.h
    constr_SEQUENCE.h
    constr_SEQUENCE_OF.h
    constr_SET_OF.h
    constr_TYPE.h
    GraphicString.h
    INTEGER.h
    M3AP_Absolute-Time-ofMBMS-Data.h
    M3AP_AllocationAndRetentionPriority.h
    M3AP_asn_constant.h
    M3AP_BitRate.h
    M3AP_Cause.h
    M3AP_CauseMisc.h
    M3AP_CauseNAS.h
    M3AP_CauseProtocol.h
    M3AP_CauseRadioNetwork.h
    M3AP_CauseTransport.h
    M3AP_CriticalityDiagnostics.h
    M3AP_CriticalityDiagnostics-IE-List.h
    M3AP_Criticality.h
    M3AP_ECGI.h
    M3AP_ErrorIndication.h
    M3AP_EUTRANCellIdentifier.h
    M3AP_ExtendedMCE-ID.h
    M3AP_EXTERNAL.h
    M3AP_GBR-QosInformation.h
    M3AP_Global-MCE-ID.h
    M3AP_GTP-TEID.h
    M3AP_InitiatingMessage.h
    M3AP_IPAddress.h
    M3AP_M3AP-PDU.h
    M3AP_M3SetupFailure.h
    M3AP_M3SetupRequest.h
    M3AP_M3SetupResponse.h
    M3AP_MBMS-Cell-List.h
    M3AP_MBMS-E-RAB-QoS-Parameters.h
    M3AP_MBMSServiceArea1.h
    M3AP_MBMS-Service-Area.h
    M3AP_MBMSServiceAreaListItem.h
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionItem.h
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionListResAck.h
    M3AP_MBMS-Service-associatedLogicalM3-ConnectionListRes.h
    M3AP_MBMS-Session-Duration.h
    M3AP_MBMS-Session-ID.h
    M3AP_MBMSSessionStartFailure.h
    M3AP_MBMSSessionStartRequest.h
    M3AP_MBMSSessionStartResponse.h
    M3AP_MBMSSessionStopRequest.h
    M3AP_MBMSSessionStopResponse.h
    M3AP_MBMSSessionUpdateFailure.h
    M3AP_MBMSSessionUpdateRequest.h
    M3AP_MBMSSessionUpdateResponse.h
    M3AP_MCEConfigurationUpdateAcknowledge.h
    M3AP_MCEConfigurationUpdateFailure.h
    M3AP_MCEConfigurationUpdate.h
    M3AP_MCE-ID.h
    M3AP_MCE-MBMS-M3AP-ID.h
    M3AP_MCEname.h
    M3AP_MinimumTimeToMBMSDataTransfer.h
    M3AP_MME-MBMS-M3AP-ID.h
    M3AP_PLMN-Identity.h
    M3AP_Pre-emptionCapability.h
    M3AP_Pre-emptionVulnerability.h
    M3AP_Presence.h
    M3AP_PriorityLevel.h
    M3AP_PrivateIE-Container.h
    M3AP_PrivateIE-Field.h
    M3AP_PrivateIE-ID.h
    M3AP_PrivateMessage.h
    M3AP_ProcedureCode.h
    M3AP_ProtocolExtensionContainer.h
    M3AP_ProtocolExtensionField.h
    M3AP_ProtocolIE-Container.h
    M3AP_ProtocolIE-ContainerList.h
    M3AP_ProtocolIE-ContainerPair.h
    M3AP_ProtocolIE-ContainerPairList.h
    M3AP_ProtocolIE-Field.h
    M3AP_ProtocolIE-FieldPair.h
    M3AP_ProtocolIE-ID.h
    M3AP_ProtocolIE-Single-Container.h
    M3AP_QCI.h
    M3AP_Reestablishment.h
    M3AP_ResetAcknowledge.h
    M3AP_ResetAll.h
    M3AP_Reset.h
    M3AP_ResetType.h
    M3AP_SuccessfulOutcome.h
    M3AP_TimeToWait.h
    M3AP_TMGI.h
    M3AP_TNL-Information.h
    M3AP_TriggeringMessage.h
    M3AP_TypeOfError.h
    M3AP_UnsuccessfulOutcome.h
    NativeEnumerated.h
    NativeInteger.h
    ObjectDescriptor.h
    OBJECT_IDENTIFIER.h
    OCTET_STRING.h
    OPEN_TYPE.h
    per_decoder.h
    per_encoder.h
    per_opentype.h
    per_support.h
    PrintableString.h
    xer_decoder.h
    xer_encoder.h
    xer_support.h
)
