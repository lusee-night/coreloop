# AppID definitions


| 0x2XX | Name                        | Priority | Description               |
|-------|-----------------------------|----------|---------------------------|
|    00 | AppID_Read_Response         | High     | RFS register read back
|    01 | AppID_Reset_Request         | High     | RFS watchdog is requesting a reset + metadata
|    05 | AppID_Registers_RB          | High     | Dumps all register values from the RFS into packet
|    06 | AppID_uC_Housekeeping       | High     | uC generater housekeeping
|    07 | AppID_End_Of_Sequence       | High     | End of sequence
|    08 | AppID_uC_Bootloader         | High     | Data from the bootloader
|    09 | AppID_uC_Start              | High     | Flight SW has booted
|    0A | AppID_uC_Heartbeat          | NONE     | Regular hearbeat packet
|    0C | AppID_Watchdog              | xxxxx    | Used for Watchdog related telemetry packets
|    0F | AppID_MetaData              | High     | x = 0..F for 16 correlations
|    1x | AppID_SpectraHigh           | High     | Main correlation products, high priorty
|    2x | AppID_SpectraMed            | Med      | Main correlation products, medium priority
|    3x | AppID_SpectraLow            | Low      | Main correlation products, low priority
|    4x | AppID_SpectraTRHigh         | High     | Time-resolved spectra, high priority
|    5x | AppID_SpectraTRMed          | Med      | Time-resolved spectra, medium priority
|    6x | AppID_SpectraTRLow          | Low      | Time-resolved Spectra, low priority
|    7x | AppID_ZoomSpectra           | High     | Spectral zoom-in spectra
|    80 | AppID_Calibrator_MetaData   | High     | Calibrator metadata packet
|    81 | AppID_Calibrator_Data       | High     | Calibrator main payload (x3)
|    84 | AppID_Calibrator_RawPFB     | High     | Calibrator raw PFB (8 packets for 4 x R/I)
|    8C | AppID_Calibrator_Debug      | High     | Calibrator debug data (8 packets)
|    Dx | AppID_SpectraVeryLow        | VeryLow  | Very low priority, not expected to be normally downloaded
|    Ex | AppID_FW_DirectSpectrum     | High     | x= 0...4 for 4 autocorrelatins
|    Fx | AppID_RawADC                | High     | x= 0..4 for 4 raw ADC data streams
