/** 
\file   dec_avc.h
\brief  AVC/H.264 Decoder API

\verbatim
File: dec_avc.h
Desc: AVC/H.264 Decoder API

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
\endverbatim
**/

#if !defined (__DEC_AVC_API_INCLUDED__)
#define __DEC_AVC_API_INCLUDED__
#include "bufstrm.h"
#include "mcdefs.h"
#include "mcapiext.h"




/** \brief Deprecated.*/
enum h264vd_error_resilience_mode {
    H264VD_ER_PROCESS_ANYWAY = 0,
    H264VD_ER_WAIT_I_SLICE,
    H264VD_ER_WAIT_IDR_SLICE
};


#define AVC_DECODER_AUTH                   0x00010082 /**< \brief Reserved  \hideinitializer */


/** \name auxiliary commands */
#define SET_ERROR_RESILIENCE_MODE   0x00010081 /**< \brief Deprecated  \hideinitializer */
#define SET_LOOP_FILTER_MODE        0x00010085 /**< \brief Set loop filter mode option \hideinitializer */
#define GET_VIEW_ID                 0x00010303 /**< \brief MVC specific \hideinitializer */
#define ENABLE_VIEW                 0x00010304 /**< \brief MVC specific \hideinitializer */
#define DISABLE_VIEW                0x00010305 /**< \brief MVC specific \hideinitializer */
#define GET_RECOVERY_POINT          0x00010087 /**< \brief Get recovery point option \hideinitializer */
#define SET_RECOVERY_POINT          0x00010088 /**< \brief Set recovery point option \hideinitializer */
#define SET_DPB_SIZE                0x00010089 /**< \brief Set DPB size option \hideinitializer */
#define SET_MV_PRECISION            0x0001008a /**< \brief Limits precision of motion vectors option \hideinitializer */
#define SET_IGNORE_CROPPING         0x000100aa /**< \brief Set ignore cropping flag, when is set to non-zero signals decoder to ignore frame cropping parameters from SPS \hideinitializer */
#define SET_ERROR_CONCEALMENT_MODE  DEC_AVC_RESERVED_00010302 /**< \brief Set  error concealment mode option \hideinitializer */
/** \} */

/** \brief Loop filter modes.  */
enum h264vd_loop_filter_mode {
    H264VD_LF_OFF = 0,              /**< Filtering will be skipped. */
    H264VD_LF_STANDARD = 1,         /**< Filtering will be applied in according with stream parameters. */
    H264VD_LF_REFERENCE_ONLY = 2    /**< Filtering will be applied for frames which are marked as 'reference'. */
};
/**\} */


/** \brief User data packet */
typedef struct user_data_packet_s{
    uint8_t *data;          /**< Data */
    int32_t payload_size;   /**< Payload data size */
}user_data_packet_t;
/** \} */

/** \brief User data */
typedef struct user_data_s{
    user_data_packet_t *user_data_packet;   /**< Vector of user data packets*/
    int32_t num_user_data_packets;          /**< Number of user data packets*/
}user_data_t;
/** \} */

/** \brief SEI message */
typedef struct sei_payload_s{
    uint8_t *payload;           /**< Payload data*/
    int32_t payload_type;       /**< Type of SEI message*/
    int32_t payload_size;       /**< Size of payload data*/
}sei_payload_t;
/** \} */

/** \brief SEI data */
typedef struct sei_message_s{
    sei_payload_t *sei_payload;     /**< Vector of SEI messages*/
    int32_t num_messages;           /**< Number of messages*/
}sei_message_t;
/** \} */



/** \brief Motion vector precision limits */
enum h264vd_mv_precision {
    H264VD_MV_PRECISION_QUARTER = 0,    /**< allow up to quater pel precision*/
    H264VD_MV_PRECISION_HALF = 1,       /**< allow up to half  pel precision*/
    H264VD_MV_PRECISION_FULL = 2        /**< allow only full pel precision*/
};
/**\} */



/** \brief Error concealemnt modes */
enum h264vd_error_concealment_mode {
    H264VD_EC_NONE                      = 0x00000000,   /**Decodes and shows all frames*/
    H264VD_EC_SHOW_BASE_LAYER           = 0x00000001,   /**< Switch to base layer if it doesn't contain errors(make sense for SVC streams only)*/
    H264VD_EC_CONCEAL_ERROR_FRAMES      = 0x00000002,   /**< Conceal errors*/
    /**< Following flags are irrelevant if H264VD_EC_CONCEAL_ERROR_FRAMES is not set*/
    H264VD_EC_TEMPORAL_FOR_INTRA        = 0x00000004,   /**< Allow inter-frames concealment for intra frames/MBs*/
    H264VD_EC_ERRORLESS_REF_ONLY        = 0x00000008    /**< Use errorless frames only at inter-frames concealment*/
};
/**\} */

/** \name auxillery commands */
#define GET_DPB_SIZE                0x0001008b /**< \brief Get  DPB size option \hideinitializer */

#define SET_NUM_UNUSED_FRAMES       0x0001008c /**< \brief Set  number of skipped frames option \hideinitializer */

#define SET_8BIT_OUTPUT             0x0001008d /**< \brief Allow only 8bit output image option \hideinitializer */

#define GET_H264SEQ_PAR_SET         0x00010091 /**< \brief Get SPS information \hideinitializer */
#define GET_H264PIC_PAR_SET         0x00010092 /**< \brief Get PPS information \hideinitializer */
/** \} */

//#define ANALYSE_OPTIONS           0x00010093
//#define ANALYSE_MB                0x00000001
//#define ANALYSE_INTRA             0x00000002
//#define ANALYSE_INTER             0x00000004
//#define ANALYSE_SCOEFF            0x00000008
//#define ANALYSE_RESDIFF           0x00000010
//#define ANALYSE_LEVEL             0x00000020
//#define ANALYSE_BITSTREAM         0x00000040      // coded bitstream extraction
//#define ANALYSE_GET_PIC           0x00010094

/** \brief NAL unit structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_nalu_s {
    int32_t forbidden_zero_bit;
    int32_t nal_ref_idc;
    int32_t nal_unit_type;
    int32_t reserved_one_bit;
    int32_t idr_flag;
    int32_t priority_id;
    int32_t no_inter_layer_pred_flag;
    int32_t dependency_id;
    int32_t quality_id;
    int32_t temporal_id;
    int32_t use_ref_base_pic_flag;
    int32_t discardable_flag;
    int32_t output_flag;
    int32_t reserved_three_2bits;
    int32_t store_ref_base_pic_flag;
    int32_t prefix_nal_unit_additional_extension_flag;
    int32_t svc_extension_flag;
    int32_t non_idr_flag;
    int32_t view_id;
    int32_t anchor_pic_flag;
    int32_t inter_view_flag;
}h264_nalu_t;
/** \} */

/** \brief HRD parameters structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_hrd_s {
    int32_t cpb_cnt_minus1;
    int32_t bit_rate_scale;
    int32_t cpb_size_scale;
    uint32_t bit_rate_value_minus1[32];
    uint32_t cpb_size_value_minus1[32];
    int32_t cbr_flag[32];
    int32_t initial_cpb_removal_delay_length_minus1;
    int32_t cpb_removal_delay_length_minus1;
    int32_t dpb_output_delay_length_minus1;
    int32_t time_offset_length;
}h264_hrd_t;
/** \} */

/** \brief VUI parameters structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_vui_s {
    int32_t aspect_ratio_info_present_flag;
    int32_t aspect_ratio_idc;
    int32_t sar_width;
    int32_t sar_height;
    int32_t overscan_info_present_flag;
    int32_t overscan_appropriate_flag;
    int32_t video_signal_type_present_flag;
    int32_t video_format;
    int32_t video_full_range_flag;
    int32_t colour_description_present_flag;
    int32_t colour_primaries;
    int32_t transfer_characteristics;
    int32_t matrix_coefficients;
    int32_t chroma_loc_info_present_flag;
    int32_t chroma_sample_loc_type_top_field;
    int32_t chroma_sample_loc_type_bottom_field;
    int32_t timing_info_present_flag;
    uint32_t num_units_in_tick;
    uint32_t time_scale;
    int32_t fixed_frame_rate_flag;
    int32_t nal_hrd_parameters_present_flag;
    h264_hrd_t nal_hrd_parameters;
    int32_t vcl_hrd_parameters_present_flag;
    h264_hrd_t vcl_hrd_parameters;
    int32_t low_delay_hrd_flag;
    int32_t pic_struct_present_flag;
    int32_t bitstream_restriction_flag;
    int32_t motion_vectors_over_pic_boundaries_flag;
    int32_t max_bytes_per_pic_denom;
    int32_t max_bits_per_mb_denom;
    int32_t log2_max_mv_length_horizontal;
    int32_t log2_max_mv_length_vertical;
    int32_t num_reorder_frames;
    int32_t max_dec_frame_buffering;
}h264_vui_t;
/** \} */

/** \brief Sequence paramter set structure.  See ISO/IEC 14496-10 for reference. */
typedef struct h264_seq_par_set_s {
    h264_nalu_t nalu;
    int32_t profile_idc;
    int32_t constraint_set0_flag;
    int32_t constraint_set1_flag;
    int32_t constraint_set2_flag;
    int32_t constraint_set3_flag;
    int32_t constraint_set4_flag;
    int32_t constraint_set5_flag;
    int32_t level_idc;
    int32_t reserved_zero_bits;
    int32_t seq_parameter_set_id;
    int32_t chroma_format_idc;
    int32_t separate_colour_plane_flag;
    int32_t bit_depth_luma_minus8;
    int32_t bit_depth_chroma_minus8;
    int32_t qpprime_y_zero_transform_bypass_flag;
    int32_t seq_scaling_matrix_present_flag;
    int32_t seq_scaling_list_present_flag[12];
    int32_t scaling_list_4x4[6][16];
    int32_t scaling_list_8x8[6][64];
    int32_t log2_max_frame_num_minus4;
    int32_t pic_order_cnt_type;
    int32_t log2_max_pic_order_cnt_lsb_minus4;
    int32_t delta_pic_order_always_zero_flag;
    int32_t offset_for_non_ref_pic;
    int32_t offset_for_top_to_bottom_field;
    int32_t num_ref_frames_in_pic_order_cnt_cycle;
    int32_t offset_for_ref_frame[8];
    int32_t num_ref_frames;
    int32_t gaps_in_frame_num_value_allowed_flag;
    int32_t pic_width_in_mbs_minus1;
    int32_t pic_height_in_map_units_minus1;
    int32_t frame_mbs_only_flag;
    int32_t mb_adaptive_frame_field_flag;
    int32_t direct_8x8_inference_flag;
    int32_t frame_cropping_flag;
    int32_t frame_crop_left_offset;
    int32_t frame_crop_right_offset;
    int32_t frame_crop_top_offset;
    int32_t frame_crop_bottom_offset;
    int32_t vui_parameters_present_flag;
    h264_vui_t vui_parameters;
    int32_t inter_layer_deblocking_filter_control_present_flag;
    int32_t extended_spatial_scalability;
    int32_t chroma_phase_x_plus1_flag;
    int32_t chroma_phase_y_plus1;
    int32_t seq_ref_layer_chroma_phase_x_plus1_flag;
    int32_t seq_ref_layer_chroma_phase_y_plus1;
    int32_t seq_scaled_ref_layer_left_offset;
    int32_t seq_scaled_ref_layer_top_offset;
    int32_t seq_scaled_ref_layer_right_offset;
    int32_t seq_scaled_ref_layer_bottom_offset;
    int32_t seq_tcoeff_level_prediction_flag;
    int32_t adaptive_tcoeff_level_prediction_flag;
    int32_t slice_header_restriction_flag;
    int32_t svc_vui_parameters_present_flag;
    int32_t additional_extension2_flag;
    int32_t additional_extension2_data_flag;
    int32_t bit_equal_to_one;
    int32_t num_views_minus1;
    int16_t *view_id;
    uint8_t *num_anchor_refs[2];
    int16_t **anchor_ref[2];
    uint8_t *num_non_anchor_refs[2];
    int16_t **non_anchor_ref[2];
    int32_t num_level_values_signalled_minus1;
    uint8_t mvc_level_idc[64];
    int16_t num_applicable_ops_minus1[64];
    uint8_t *applicable_op_temporal_id[64];
    int16_t *applicable_op_num_target_views_minus1[64];
    int16_t **applicable_op_target_view_id[64];
    int16_t *applicable_op_num_views_minus1[64];
    int32_t mvc_vui_parameters_present_flag;
}h264_seq_par_set_t;
/** \} */

/** \brief Short sequence paramter set structure.  See ISO/IEC 14496-10 for reference. */
typedef struct h264_seq_par_set_short_s {
    int32_t profile_idc;
    int32_t level_idc;
    int32_t seq_parameter_set_id;
    int32_t chroma_format_idc;
    int32_t bit_depth_luma_minus8;
    int32_t bit_depth_chroma_minus8;
    int32_t num_ref_frames;
    int32_t pic_width_in_mbs_minus1;
    int32_t pic_height_in_map_units_minus1;
    int32_t mb_adaptive_frame_field_flag;
    int32_t frame_cropping_flag;
    int32_t frame_crop_left_offset;
    int32_t frame_crop_right_offset;
    int32_t frame_crop_top_offset;
    int32_t frame_crop_bottom_offset;
    int32_t vui_parameters_present_flag;
    /** \brief Short VUI set structure.  See ISO/IEC 14496-10 for reference. */
    struct h264_vui_short_t {
        int32_t aspect_ratio_info_present_flag;
        int32_t aspect_ratio_idc;
        int32_t sar_width;
        int32_t sar_height;
        int32_t video_signal_type_present_flag;
        int32_t video_format;
        int32_t video_full_range_flag;
        int32_t colour_description_present_flag;
        int32_t colour_primaries;
        int32_t transfer_characteristics;
        int32_t matrix_coefficients;
        int32_t timing_info_present_flag;
        uint32_t num_units_in_tick;
        uint32_t time_scale;
        int32_t fixed_frame_rate_flag;
        int32_t reserved[1024];
    } vui_parameters;
    /** \} */
    int32_t num_views_minus1;
    int32_t reserved[4096];
}h264_seq_par_set_short_t;
/** \} */

/** \brief Picture parameter set structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_pic_par_set_s {
    h264_nalu_t nalu;
    int32_t pic_parameter_set_id;
    int32_t seq_parameter_set_id;
    int32_t entropy_coding_mode_flag;
    int32_t pic_order_present_flag;
    int32_t num_slice_groups_minus1;
    int32_t slice_group_map_type;
    int32_t run_length_minus1[8];
    int32_t top_left[8];
    int32_t bottom_right[8];
    int32_t slice_group_change_direction_flag;
    int32_t slice_group_change_rate_minus1;
    int32_t pic_size_in_map_units_minus1;
    uint8_t *slice_group_id;
    int32_t num_ref_idx_l0_active_minus1;
    int32_t num_ref_idx_l1_active_minus1;
    int32_t weighted_pred_flag;
    int32_t weighted_bipred_idc;
    int32_t pic_init_qp_minus26;
    int32_t pic_init_qs_minus26;
    int32_t chroma_qp_index_offset;
    int32_t second_chroma_qp_index_offset;
    int32_t deblocking_filter_control_present_flag;
    int32_t constrained_intra_pred_flag;
    int32_t redundant_pic_cnt_present_flag;
    int32_t transform_8x8_mode_flag;
    int32_t pic_scaling_matrix_present_flag;
    int32_t pic_scaling_list_present_flag[12];
    int32_t scaling_list_4x4[6][16];
    int32_t scaling_list_8x8[6][64];
}h264_pic_par_set_t;
/**\} */

/** \brief SEI pic timing structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_sei_pic_timing_s {
    uint32_t cpb_removal_delay;
    uint32_t dpb_output_delay;
    int32_t pic_struct;
    int32_t clock_timestamp_flag[3];
    int32_t ct_type;
    int32_t nuit_field_based_flag;
    int32_t counting_type;
    int32_t full_timestamp_flag;
    int32_t discontinuity_flag;
    int32_t cnt_dropped_flag;
    int32_t n_frames;
    int32_t seconds_flag;
    int32_t seconds_value;
    int32_t minutes_flag;
    int32_t minutes_value;
    int32_t hours_flag;
    int32_t hours_value;
    int32_t time_offset;
}h264_sei_pic_timing_t;
/** \} */

/** \brief SEI buffering period structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_sei_buffering_period_s {
    int32_t seq_paramater_set_id;
    uint32_t nal_initial_cpb_removal_delay[32];
    uint32_t nal_initial_cpb_removal_delay_offset[32];
    uint32_t vcl_initial_cpb_removal_delay[32];
    uint32_t vcl_initial_cpb_removal_delay_offset[32];
}h264_sei_buffering_period_t;
/** \} */

/** \brief SEI recovering point structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_sei_recovery_point_s {
    int32_t recovery_frame_cnt;
    int32_t exact_match_flag;
    int32_t broken_link_flag;
    int32_t changing_slice_group_idc;
}h264_sei_recovery_point_t;
/** \} */

/** \brief SEI user data unregistered. See ISO/IEC 14496-10 for reference. */
typedef struct h264_sei_unregistered_s {
    uint8_t uuid_iso_iec_11578[16];
    void *p;
    uint32_t len;
    uint32_t reserved[1024];
}h264_sei_unregistered_t;
/** \} */

/** \brief SEI structure */
typedef struct h264_sei_s {
    void *p;                    /**< SEI payload data*/
    uint32_t payload_type;      /**< SEI payload type*/
}h264_sei_t;
/** \} */

/** \brief Reference pic reoreding/modification information structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_ref_pic_list_reordering_s {
    int32_t ref_pic_list_reordering_flag[2];
    int32_t reordering_of_pic_nums_idc[2][32];
    union {
        int32_t abs_diff_pic_num_minus1[2][32];
        int32_t long_term_pic_num[2][32];
        int32_t abs_diff_view_idx_minus1[2][32];
    };
}h264_ref_pic_list_reordering_t;
/** \} */

/** \brief MMCO information structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_dec_ref_pic_marking_s {
    int32_t memory_management_control_operation;
    int32_t difference_of_pic_nums_minus1;
    union {
        int32_t long_term_pic_num;
        int32_t long_term_frame_idx;
        int32_t max_long_term_frame_idx_plus1;
    };
}h264_dec_ref_pic_marking_t;
/** \} */

/** \brief Prediction weight structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_pred_weight_s {
    int32_t luma_log2_weight_denom;
    int32_t chroma_log2_weight_denom;
    int8_t luma_weight_flag[2][32];
    int8_t chroma_weight_flag[2][32];
    int16_t luma_weight[2][32];
    int16_t luma_offset[2][32];
    int16_t chroma_weight[2][32][2];
    int16_t chroma_offset[2][32][2];
}h264_pred_weight_t;
/** \} */
 
/** \brief Slice header structure. See ISO/IEC 14496-10 for reference. */
typedef struct h264_slice_header_s {
    h264_nalu_t nalu;
    int32_t first_mb_in_slice;
    int32_t slice_type;
    int32_t pic_parameter_set_id;
    int32_t colour_plane_id;
    int32_t frame_num;
    int32_t field_pic_flag;
    int32_t bottom_field_flag;
    int32_t idr_pic_id;
    int32_t pic_order_cnt_lsb;
    int32_t delta_pic_order_cnt_bottom;
    int32_t delta_pic_order_cnt[2];
    int32_t redundant_pic_cnt;
    int32_t direct_spatial_mv_pred_flag;
    int32_t num_ref_idx_active_override_flag;
    int32_t num_ref_idx_l0_active_minus1;
    int32_t num_ref_idx_l1_active_minus1;
    int32_t cabac_init_idc;
    int32_t slice_qp_delta;
    int32_t sp_for_switch_flag;
    int32_t slice_qs_delta;
    int32_t disable_deblocking_filter_idc;
    int32_t slice_alpha_c0_offset_div2;
    int32_t slice_beta_offset_div2;
    int32_t slice_group_change_cycle;
    int32_t no_output_of_prior_pics_flag;
    int32_t long_term_reference_flag;
    int32_t adaptive_ref_pic_buffering_flag;
    int32_t no_inter_layer_pred_flag;
    int32_t quality_id;
    int32_t base_mmco_count;
    int32_t base_pred_weight_table_flag;
    int32_t store_ref_base_pic_flag;
    int32_t base_adaptive_ref_pic_marking_mode_flag;
    int32_t ref_layer_dq_id;
    int32_t disable_inter_layer_deblocking_filter_idc;
    int32_t inter_layer_slice_alpha_c0_offset_div2;
    int32_t inter_layer_slice_beta_offset_div2;
    int32_t constrained_intra_resampling_flag;
    int32_t ref_layer_chroma_phase_x_plus1_flag;
    int32_t ref_layer_chroma_phase_y_plus1;
    int32_t scaled_ref_layer_left_offset;
    int32_t scaled_ref_layer_top_offset;
    int32_t scaled_ref_layer_right_offset;
    int32_t scaled_ref_layer_bottom_offset;
    int32_t slice_skip_flag;
    int32_t num_mbs_in_slice_minus1;
    int32_t adaptive_prediction_flag;
    int32_t default_base_mode_flag;
    int32_t adaptive_motion_prediction_flag;
    int32_t default_motion_prediction_flag;
    int32_t adaptive_residual_prediction_flag;
    int32_t default_residual_prediction_flag;
    int32_t tcoeff_level_prediction_flag;
    int32_t scan_idx_start;
    int32_t scan_idx_end;
    int32_t slice_header_first_bit_offset;
    int32_t slice_data_first_bit_offset;
    h264_ref_pic_list_reordering_t ref_pic_list_reordering;
    h264_pred_weight_t pred_weight_table;
    h264_dec_ref_pic_marking_t dec_ref_pic_marking[67];
    h264_dec_ref_pic_marking_t base_dec_ref_pic_marking[67];
}h264_slice_header_t;
/**\} */

/** \brief Macroblock layer and prediction information. See ISO/IEC 14496-10 for reference. */
typedef struct h264_macroblock_s {
    int8_t mb_skip_flag;
    int8_t mb_field_decoding_flag;
    int8_t mb_type;
    int8_t transform_size_8x8_flag;
    int8_t intra_chroma_pred_mode;
    int8_t sub_mb_type[4];
    int8_t coded_block_pattern;
    uint32_t coded_block_pattern_ext[2];
    int8_t mb_qp_delta;

    //non-coded parameters
    int32_t slice_nr;
    int8_t  qp[3];
    uint16_t bits_mb;
    uint16_t bits_residual;
    uint16_t bits_residual_chroma;

    int32_t reserved[3];
}h264_macroblock_t;
/** \} */

/** \brief Motion vectors */
typedef struct h264_mv_s {
    int16_t xy[16][2];
}h264_mv_t;
/** \} */

/** \brief Reference picture information */
typedef struct h264_ref_pic_s {
    int32_t id;
    int32_t bottom_field_flag;
    int32_t short_term_reference_flags;
    int32_t long_term_reference_flags;
    int32_t long_term_frame_idx;
    int32_t non_existing_flag;
}h264_ref_pic_t;
/** \} */

/** \brief Refence picture list */
typedef struct h264_dpb_s {
    h264_ref_pic_t ref_pic[16];
}h264_dpb_t;
/** \} */

/** \brief Slice information */
typedef struct h264_slice_s {
    h264_slice_header_t *slice_header;
    
    int32_t ref_count[2];
    h264_ref_pic_t ref_pic[2][32];
    
    uint8_t *slice_data;
    uint32_t bytes_in_slice;

    uint8_t reserved[512];
}h264_slice_t;
/** \} */

/** \brief Modified DV Pack Meta. See AVCHD Spec Book 2 Section 4.6 */
typedef struct h264_sei_modified_dv_pack_meta_s {

    uint32_t number_of_entries;

    uint32_t time_code_present_flag;
    mc_time_code_t time_code;

    uint32_t binary_group_present_flag;
    struct binary_group_s{
        uint8_t bg[8];
    }binary_group;

    uint32_t rec_date_and_time_present_flag;
    struct rec_date_and_time_s{
        uint32_t daylight_saving_flag;
        uint32_t time_zone_sign;
        uint32_t time_zone_val;
        uint32_t half_hour_flag;
        uint16_t year;
        uint32_t month;
        uint32_t day;
        uint32_t hour;
        uint32_t minute;
        uint32_t second;
    } rec_date_and_time;

    uint32_t consumer_camera_1_present_flag;
    struct consumer_camera_1_s{
        uint32_t iris;
        uint32_t ae_mode;
        uint32_t agc;
        uint32_t wb_mode;
        uint32_t white_balance;
        uint32_t fcm;
        uint32_t focus;
    } consumer_camera_1;

    uint32_t consumer_camera_2_present_flag;
    struct consumer_camera_2_s{
        uint32_t vpd;
        uint32_t v_pannig_speed;
        uint32_t is;
        uint32_t hpd;
        uint32_t h_panning_speed;
        uint32_t focal_length;
        uint32_t zen;
        uint32_t units_of_e_zoom;
        uint32_t one_to_ten_of_e_zoom;
    } consumer_camera_2;

    uint32_t lens_present_flag;
    struct lens_s{
        uint32_t fcm;
        uint32_t focus;
        uint32_t iris;
        uint32_t zoom;
        uint32_t extender;
        uint32_t iris_cont;
    }lens;

    uint32_t gain_present_flag;
    struct gain_s{
        uint32_t gm;
        uint32_t master_gain;
        uint32_t r_gain;
        uint32_t b_gain;
        uint32_t nd_filter;
        uint32_t cc_filter;
    }gain;

    uint32_t pedestal_present_flag;
    struct pedestal_s{
        uint32_t g;
        uint32_t r;
        uint32_t b;
        uint32_t qf;
    }pedestal;

    uint32_t gamma_present_flag;
    struct gamma_s{
        uint32_t g;
        uint32_t r;
        uint32_t b;
        uint32_t qf;
    }gamma;

    uint32_t detail_present_flag;
    struct detail_s{
        uint32_t master;
        uint32_t h;
        uint32_t v;
        uint32_t qf;
    }detail;

    uint32_t camera_preset_present_flag;
    uint32_t camera_preset_shutter_speed;

    uint32_t flare_present_flag;
    struct flare_s{
        uint32_t g;
        uint32_t r;
        uint32_t b;
        uint32_t qf;
    }flare;

    uint32_t shading_present_flag[2];
    struct shading_s{
        uint32_t shad_comp_1_0;
        uint32_t ch0;
        uint32_t shad_comp_3_2;
        uint32_t ch1;
        uint32_t shad_comp_4_5;
        uint32_t wb;
        uint32_t shad_comp_6_7;
    }shading[2];

    uint32_t knee_present_flag;
    struct knee_s{
        uint32_t point;
        uint32_t slope;
    }knee;

    uint32_t shutter_present_flag;
    uint32_t shutter_speed;

    uint32_t exposure_time_present_flag;
    float exposure_time;

    uint32_t f_number_present_flag;
    float f_number;

    uint32_t exposure_program_present_flag;
    uint32_t exposure_program;

    uint32_t brightness_present_flag;
    float brightness;

    uint32_t exposure_bias_present_flag;
    float exposure_bias;

    uint32_t max_aperture_present_flag;
    float max_aperture;

    uint32_t flash_present_flag;
    uint32_t flash;

    uint32_t custom_rendered_present_flag;
    uint32_t custom_rendered;

    uint32_t white_balance_present_flag;
    uint32_t white_balance;

    uint32_t focal_length_in_35mm_present_flag;
    uint32_t focal_length_in_35mm;

    uint32_t scene_capture_type_present_flag;
    uint32_t scene_capture_type;

    uint32_t exif_option_present_flag;
    uint32_t exif_option_entries;
    uint32_t exif_option[5];

    struct gps_s{
        uint32_t version_present_flag;
        uint32_t version[4];

        uint32_t latitude_present_flag;
        uint8_t latitude_ref;
        float latitude[3]; 

        uint32_t longtitude_present_flag;
        uint8_t longtitude_ref;
        float longtitude[3]; 

        uint32_t altitude_present_flag;
        uint8_t altitude_ref;
        float altitude; 

        uint32_t timestamp_present_flag;
        float timestamp[3]; 

        uint32_t status_present_flag;
        uint8_t status;

        uint32_t measure_mode_present_flag;
        uint8_t measure_mode;

        uint32_t dop_present_flag;
        float dop;

        uint32_t speed_present_flag;
        uint8_t speed_ref;
        float speed;

        uint32_t track_present_flag;
        uint8_t track_ref;
        float track;

        uint32_t img_direction_present_flag;
        uint8_t img_direction_ref;
        float img_direction;

        uint32_t map_datum_present_flag;
        uint8_t map_datum[8];

        uint32_t option_present_flag;
        uint32_t option_entries;
        uint32_t option[7];
    }gps;

    uint32_t maker_and_model_present_flag;
    uint32_t maker_id;
    uint32_t maker_model_code;

    uint32_t maker_private_data_present_flag;
    uint32_t maker_private_data_entries;
    uint32_t maker_private_data[15];

}h264_sei_modified_dv_pack_meta_t;
/** \} */

typedef struct h264_frame_s h264_frame_t;
/** \brief Picture information */
typedef struct h264_picture_s {
    h264_frame_t        *frame;//parant frame
    h264_pic_par_set_t  *pic_par_set;
    h264_sei_t          *sei;
    h264_slice_t        *slice;

    uint32_t    id;
    int32_t     sei_count;
    int32_t     slice_count;
    int32_t     poc;
    int32_t     first_pic_flag;

    h264_macroblock_t   *mb;
    
    int8_t      *ipred;
    h264_mv_t   *mvd[2];
    h264_mv_t   *mv[2];
    int8_t      *ref_idx[2];
    void        *level[3];
    void        *coeff[3];
    void        *residual[3];
    
    void    *samples_predicted[3];
    int32_t stride_samples_predicted[3];

    void    *samples_restored[3];
    int32_t stride_samples_restored[3];

    void    *samples[3];
    int32_t stride_samples[3];

    uint8_t qmatrix_4x4[6][16];
    uint8_t qmatrix_8x8[6][64];

    uint8_t reserved[512];
}h264_picture_t;
/** \} */

/** \brief Frame information */
struct h264_frame_s {
    h264_seq_par_set_t  *seq_par_set;
    h264_picture_t      *pic[2];

    int32_t id;
    int32_t interlaced_frame_flag;
    int32_t frame_num;
    int32_t poc[2];

    int32_t short_term_reference_flags;
    int32_t long_term_reference_flags;

    uint8_t reserved[512];
};
/** \} */

/*
sample usage of streaming H264-decoder

bufstream_tt *bs;
struct SEQ_Params *pSEQ;
struct PIC_Params *pPIC;
frame_tt output_frame;
unsigned long state;
int n;

// Create the h.264 parser.
bs = open_h264in_Video_stream();
// Init the parser to start using it.
bs->auxinfo(bs,0,PARSE_INIT,NULL,0);
// Set the parser to parsing frames mode.
bs->auxinfo(bs,0,PARSE_FRAMES,NULL,0);

while(chunk_size > 0) {
    // Put chunk of coded stream to the parser.
    n = bs->copybytes(bs,chunk_ptr,chunk_size);
    chunk_ptr += n;
    chunk_size -= n;
    
    // Get and clean parser's state.
    state = bs->auxinfo(bs,0,CLEAN_PARSE_STATE,NULL,0);

    // Check parser's state.
    if( state & (PIC_VALID_FLAG|PIC_FULL_FLAG) ) {
        // Get the headers to which this frame refers.
        // SEQ_Params contains information about frame resolution.
        // PIC_Params contains information about frame type and its POC.
        dec->auxinfo(dec,0,GET_SEQ_PARAMSP,&pSEQ,sizeof(struct SEQ_Params));
        dec->auxinfo(dec,0,GET_PIC_PARAMSP,&pPIC,sizeof(struct PIC_Params));

        // Set color format in which you would like to get the frame
        // and pointers to samples' buffer in frame_tt structure.
        output_frame.four_cc = MAKEFOURCC('Y','V','1','2');
        output_frame.plane[0] = ptr_Y;
        output_frame.plane[1] = ptr_V;
        output_frame.plane[2] = ptr_U;
        output_frame.stride[0] = stride_Y;
        output_frame.stride[1] = stride_V;
        output_frame.stride[2] = stride_U;
        output_frame.width = pSEQ->horizontal_size;
        output_frame.height = pSEQ->vertical_size;

        if( BS_OK==dec->auxinfo(dec,0,GET_PIC,&output_frame,sizeof(frame_tt)) ) {
            //..
        }
    }
}

// Flush the decoder to get all pended frames.
do {
    n = bs->copybytes(bs,NULL,0);
    state = bs->auxinfo(bs,0,CLEAN_PARSE_STATE,NULL,0);
    if( state & (PIC_VALID_FLAG|PIC_FULL_FLAG) ) {
        //...
    }
}while(n);

// Destroy instance.
bs->free(bs);

*/


#ifdef __cplusplus
extern "C" {
#endif

/**
 \brief Call this function to get a bufstream interface for the H.264 video decoder.
 \return  The video decoder instance.
 */
    bufstream_tt *  MC_EXPORT_API open_h264in_Video_stream(void); //quick and dirty default initialization
    
/**
 \brief Call this function to get a bufstream interface for the H.264 video decoder.
 \return  The video decoder instance.
 \param[in] get_rc  callbacks to external message handler and memory manager functions
 \param[in] reserved1 reserved parameter
 \param[in] reserved2 reserved parameter
 */
    bufstream_tt *  MC_EXPORT_API open_h264in_Video_stream_ex(void *(MC_EXPORT_API *get_rc)(const char* name), long reserved1, long reserved2);

/**
\brief Provides access to extended module API.
\return  pointer to requested function or NULL
\param[in] func identifier of module extended function
 */
    APIEXTFUNC      MC_EXPORT_API h264in_Video_GetAPIExt(uint32_t func);
    
#ifdef __cplusplus
}
#endif

#endif  // #if !defined (__ENC_AVC_API_INCLUDED__)
