#ifndef _RT5640_CONFIG_H_
#define _RT5640_CONFIG_H_

#include "config.h"

const struct config_control rt5640_speaker_normal_controls[] = {
    {
        .ctl_name = "DAI select",
        .str_val = "1:2|2:1",
    },
    {
        .ctl_name = "Mono DAC Playback Volume",
        .int_val = {175, 175},
    },
    {
        .ctl_name = "DAC2 Playback Switch",
        .int_val = {on, on},
    },
    {
        .ctl_name = "Mono DAC MIXL DAC L2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "Mono DAC MIXR DAC R2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HPO MIX DAC2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HP L Playback Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HP R Playback Switch",
        .int_val = {on},
    },

};

const struct config_control rt5640_headphone_normal_controls[] = {
    {
        .ctl_name = "DAI select",
        .str_val = "1:2|2:1",
    },
    {
        .ctl_name = "Mono DAC Playback Volume",
        .int_val = {175, 175},
    },
    {
        .ctl_name = "DAC2 Playback Switch",
        .int_val = {on, on},
    },
    {
        .ctl_name = "Mono DAC MIXL DAC L2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "Mono DAC MIXR DAC R2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HPO MIX DAC2 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HP L Playback Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "HP R Playback Switch",
        .int_val = {on},
    },
};

const struct config_control rt5640_main_mic_capture_controls[] = {
    {
        .ctl_name = "DAI select",
        .str_val = "1:2|2:1",
    },
    {
        .ctl_name = "RECMIXR BST3 Switch",
        .int_val = {off},
    },
    {
        .ctl_name = "RECMIXR BST1 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "Mono ADC R1 Mux",
        .str_val = "ADCR",
    },
    {
        .ctl_name = "Mono ADC MIXR ADC1 Switch",
        .int_val = {on},
    },
    {
        .ctl_name = "ADC IF2 Data Switch",
        .str_val = "right copy to left",
    },
    //min=0,max=8, bypass=0=0db, 30db=3, 52db=8
    {
        .ctl_name = "IN1 Boost",
        .int_val = {5},
    },
    //dBscale-min=-17.625dB,step=0.375dB,min=0,max=127
    {
        .ctl_name = "Mono ADC Capture Volume",
        .int_val = {60, 60},
    },
};

const struct config_control rt5640_playback_off_controls[] = {
    {
        .ctl_name = "HP L Playback Switch",
        .int_val = {off},
    },
    {
        .ctl_name = "HP R Playback Switch",
        .int_val = {off},
    },

};

const struct config_control rt5640_capture_off_controls[] = {
    {
        .ctl_name = "RECMIXR BST1 Switch",
        .int_val = {off},
    },
    {
        .ctl_name = "RECMIXR BST3 Switch",
        .int_val = {off},
    },
};


const struct config_route_table rt5640_config_table = {
    //speaker
    .speaker_normal = {
        .sound_card = 0,
        .devices = DEVICES_0,
        .controls = rt5640_speaker_normal_controls,
        .controls_count = sizeof(rt5640_speaker_normal_controls) / sizeof(struct config_control),
    },

    //headphone
    .headphone_normal = {
        .sound_card = 0,
        .devices = DEVICES_0,
        .controls = rt5640_headphone_normal_controls,
        .controls_count = sizeof(rt5640_headphone_normal_controls) / sizeof(struct config_control),
    },

    //capture
    .main_mic_capture = {
        .sound_card = 0,
        .devices = DEVICES_0,
        .controls = rt5640_main_mic_capture_controls,
        .controls_count = sizeof(rt5640_main_mic_capture_controls) / sizeof(struct config_control),
    },

    //off
    .playback_off = {
        .controls = rt5640_playback_off_controls,
        .controls_count = sizeof(rt5640_playback_off_controls) / sizeof(struct config_control),
    },
    .capture_off = {
        .controls = rt5640_capture_off_controls,
        .controls_count = sizeof(rt5640_capture_off_controls) / sizeof(struct config_control),
    },

    //hdmi
    .hdmi_normal = {
        .sound_card = 1,
        .devices = DEVICES_0,
        .controls_count = 0,
    },

    //usb audio
    .usb_normal = {
        .sound_card = 2,
        .devices = DEVICES_0,
        .controls_count = 0,
    },
    .usb_capture = {
        .sound_card = 2,
        .devices = DEVICES_0,
        .controls_count = 0,
    },
};

#endif //_RT5640_CONFIG_H_
