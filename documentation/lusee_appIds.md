# AppID definitions


| 0x2XX | Name                        | Priority | Description               |
|-------|-----------------------------|----------|---------------------------|
|    00 | AppID Read Response         | High     | RFS register read back
|    01 | AppID_Reset_Request         | High     | RFS watchdog is requesting a reset + metadata
|    05 | AppID_Registers_RB          | High     | Dumps all register values from the RFS into packet
|    06 | AppID_uC_Housekeeping       | High     | uC generater housekeeping
|    07 | AppID_Calibrator_Detect     | High     | Calibrator detected with meta data
|    08 | AppID_uC_Bootloader         | High     | Data from the bootloader
|    09 | AppID_uC_Start              | High     | Flight SW has booted
|    0A | AppID_uC_HeartBeat          | None     | HeartBeat
|    0B | AppID_uC_Sequencer_complete | High     | For sequencer with a limited number of steps
|    0F | AppID_MetaData              | High     | x = 0..F for 16 correlations
|    1x | AppID_SpectraHigh           | High     | Main correlation products, high priorty
|    2x | AppID_SpectraMed            | Med      | Main correlation products, medium priority
|    3x | AppID_SpectraLow            | Low      | Main correlation products, low priority
|    4x | AppID_SpectraRejectHigh     | High     | Rejected spectra, high priority
|    5x | AppID_SpectraRejectMed      | Med      | Rejected spectra, medium priority
|    6x | AppID_SpectraRejectLow      | Low      | Rejected Spectra, low priority
|    7x | AppID_ZoomSpectra           | High     | Spectral zoom-in spectra
|    8x | AppID_TimeZoomSpectra       | High     | Time zoom-in spectra
|    9x | AppID_Calibrator_Data       | High     | Calibrator data
|    Dx | AppID_SpectraVeryLow        | VeryLow  | Very low priority, not expected to be normally downloaded
|    Ex | AppID_FW_DirectSpectrum     | High     | x= 0...4 for 4 autocorrelatins
|    Fx | AppID_RawADC                | High     | x= 0..4 for 4 raw ADC data streams
