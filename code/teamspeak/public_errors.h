#ifndef PUBLIC_ERRORS_H
#define PUBLIC_ERRORS_H

//The idea here is: the values are 2 bytes wide, the first byte identifies the group, the second the count within that group

//general
const unsigned int ERROR_ok                                     = 0x0000;
const unsigned int ERROR_undefined                              = 0x0001;
const unsigned int ERROR_not_implemented                        = 0x0002;
const unsigned int ERROR_ok_no_update                           = 0x0003;
const unsigned int ERROR_dont_notify                            = 0x0004;

//dunno
const unsigned int ERROR_command_not_found                      = 0x0100;
const unsigned int ERROR_unable_to_bind_network_port            = 0x0101;
const unsigned int ERROR_no_network_port_available              = 0x0102;

//client
const unsigned int ERROR_client_invalid_id                      = 0x0200;
const unsigned int ERROR_client_nickname_inuse                  = 0x0201;
const unsigned int ERROR_client_protocol_limit_reached          = 0x0203;
const unsigned int ERROR_client_invalid_type                    = 0x0204;
const unsigned int ERROR_client_already_subscribed              = 0x0205;
const unsigned int ERROR_client_not_logged_in                   = 0x0206;
const unsigned int ERROR_client_could_not_validate_identity     = 0x0207;

//channel
const unsigned int ERROR_channel_invalid_id                     = 0x0300;
const unsigned int ERROR_channel_protocol_limit_reached         = 0x0301;
const unsigned int ERROR_channel_already_in                     = 0x0302;
const unsigned int ERROR_channel_name_inuse                     = 0x0303;
const unsigned int ERROR_channel_not_empty                      = 0x0304;
const unsigned int ERROR_channel_can_not_delete_default         = 0x0305;
const unsigned int ERROR_channel_default_require_permanent      = 0x0306;
const unsigned int ERROR_channel_invalid_flags                  = 0x0307;
const unsigned int ERROR_channel_parent_not_permanent           = 0x0308;
const unsigned int ERROR_channel_maxclients_reached             = 0x0309;
const unsigned int ERROR_channel_maxfamily_reached              = 0x030a;
const unsigned int ERROR_channel_invalid_order                  = 0x030b;
const unsigned int ERROR_channel_no_filetransfer_supported      = 0x030c;
const unsigned int ERROR_channel_invalid_password               = 0x030d;

//server
const unsigned int ERROR_server_invalid_id                      = 0x0400;
const unsigned int ERROR_server_running                         = 0x0401;
const unsigned int ERROR_server_is_shutting_down                = 0x0402;
const unsigned int ERROR_server_maxclients_reached              = 0x0403;
const unsigned int ERROR_server_invalid_password                = 0x0404;
const unsigned int ERROR_server_is_virtual                      = 0x0407;

//parameter
const unsigned int ERROR_parameter_quote                        = 0x0600;
const unsigned int ERROR_parameter_invalid_count                = 0x0601;
const unsigned int ERROR_parameter_invalid                      = 0x0602;
const unsigned int ERROR_parameter_not_found                    = 0x0603;
const unsigned int ERROR_parameter_convert                      = 0x0604;
const unsigned int ERROR_parameter_invalid_size                 = 0x0605;
const unsigned int ERROR_parameter_missing                      = 0x0606;

//unsorted, need further investigation
const unsigned int ERROR_vs_critical                            = 0x0700;
const unsigned int ERROR_connection_lost                        = 0x0701;
const unsigned int ERROR_not_connected                          = 0x0702;
const unsigned int ERROR_no_cached_connection_info              = 0x0703;
const unsigned int ERROR_currently_not_possible                 = 0x0704;
const unsigned int ERROR_failed_connection_initialisation       = 0x0705;
const unsigned int ERROR_could_not_resolve_hostname             = 0x0706;
const unsigned int ERROR_invalid_server_connection_handler_id   = 0x0707;
const unsigned int ERROR_could_not_initialise_input_manager     = 0x0708;
const unsigned int ERROR_clientlibrary_not_initialised          = 0x0709;
const unsigned int ERROR_serverlibrary_not_initialised          = 0x070a;

//sound
const unsigned int ERROR_sound_preprocessor_disabled            = 0x0900;
const unsigned int ERROR_sound_internal_preprocessor            = 0x0901;
const unsigned int ERROR_sound_internal_encoder                 = 0x0902;
const unsigned int ERROR_sound_internal_playback                = 0x0903;
const unsigned int ERROR_sound_no_capture_device_available      = 0x0904;
const unsigned int ERROR_sound_no_playback_device_available     = 0x0905;
const unsigned int ERROR_sound_could_not_open_capture_device    = 0x0906;
const unsigned int ERROR_sound_could_not_open_playback_device   = 0x0907;
const unsigned int ERROR_sound_handler_has_device               = 0x0908;
const unsigned int ERROR_sound_invalid_capture_device           = 0x0909;
const unsigned int ERROR_sound_invalid_playback_device          = 0x090a;

//accounting
const unsigned int ERROR_accounting_virtualserver_limit_reached = 0x0b00;
const unsigned int ERROR_accounting_slot_limit_reached          = 0x0b01;
const unsigned int ERROR_accounting_license_file_not_found      = 0x0b02;
const unsigned int ERROR_accounting_license_date_not_ok         = 0x0b03;
const unsigned int ERROR_accounting_unable_to_connect_to_server = 0x0b04;
const unsigned int ERROR_accounting_unknown_error               = 0x0b05;
const unsigned int ERROR_accounting_server_error                = 0x0b06;
const unsigned int ERROR_accounting_instance_limit_reached      = 0x0b07;
const unsigned int ERROR_accounting_instance_check_error        = 0x0b08;
const unsigned int ERROR_accounting_license_file_invalid        = 0x0b09;
const unsigned int ERROR_accounting_running_elsewhere           = 0x0b0a;
const unsigned int ERROR_accounting_instance_duplicated         = 0x0b0b;
const unsigned int ERROR_accounting_already_started             = 0x0b0c;
const unsigned int ERROR_accounting_not_started                 = 0x0b0d;

#endif
