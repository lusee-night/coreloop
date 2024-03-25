# AppID definitions


| 0x2XX | Name                        | Priority | Description               |
|-------|-----------------------------|----------|---------------------------|
|    00 | AppID Read Response         | High     | 
|    01 | AppID_Reset_Request         | High     | 
|    05 | AppID_Registers_RB          | High     | 
|    06 | AppID_uC_Housekeeping       | High     | 
|    07 | AppID_Calibrator_Detect     | High     | 
|    08 | AppID_uC_Startup            | High     | 
|    09 | AppID_uC_Sequencer_complete | High     | 
|-------|-----------------------------|----------|----------------------------------
|    0F | AppID_MetaData              | High     | x = 0..F for 16 correlations
|    1x | AppID_SpectraHigh           | High     | 
|    2x | AppID_SpectraMed            | Med      | 
|    3x | AppID_SpectraLow            | Low      | 
|    4x | AppID_SpectraRejectHigh     | High     | 
|    5x | AppID_SpectraRejectMed      | Med      | 
|    6x | AppID_SpectraRejectLow      | Low      | 
|    7x | AppID_ZoomSpectra           | High     | 
|    8x | AppID_TimeZoomSpectra       | High     | 
|    Dx | AppID_SpectraVeryLow        | VeryLow  | Very low priority, not expected to be normally downloaded
|-------|-----------------------------|----------|---------------------------------
|    Ex | AppID_FW_DirectSpectrum     | High     | x= 0...4 for 4 autocorrelatins
|    Fx | AppID_RawADC                | High     | x= 0..4 for 4 raw ADC data streams
|-----|-----------------------------|----------|-----------------------------------