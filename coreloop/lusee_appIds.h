
#ifndef LUSEE_APPIDS_H
#define LUSEE_APPIDS_H

// RFS register read back; Priority:  High     
#define AppID_Read_Response 0x0200

// RFS watchdog is requesting a reset + metadata; Priority:  High     
#define AppID_Reset_Request 0x0201

// Dumps all register values from the RFS into packet; Priority:  High     
#define AppID_Registers_RB 0x0205

// uC generater housekeeping; Priority:  High     
#define AppID_uC_Housekeeping 0x0206

// End of sequence; Priority:  High     
#define AppID_End_Of_Sequence 0x0207

// Data from the bootloader; Priority:  High     
#define AppID_uC_Bootloader 0x0208

// Flight SW has booted; Priority:  High     
#define AppID_uC_Start 0x0209

// Regular hearbeat packet; Priority:  NONE     
#define AppID_uC_Heartbeat 0x020A

// Used for Watchdog related telemetry packets; Priority:  xxxxx    
#define AppID_Watchdog 0x020C

// x = 0..F for 16 correlations; Priority:  High     
#define AppID_MetaData 0x020F

// Main correlation products, high priorty; Priority:  High     
#define AppID_SpectraHigh 0x0210

// Main correlation products, medium priority; Priority:  Med      
#define AppID_SpectraMed 0x0220

// Main correlation products, low priority; Priority:  Low      
#define AppID_SpectraLow 0x0230

// Time-resolved spectra, high priority; Priority:  High     
#define AppID_SpectraTRHigh 0x0240

// Time-resolved spectra, medium priority; Priority:  Med      
#define AppID_SpectraTRMed 0x0250

// Time-resolved Spectra, low priority; Priority:  Low      
#define AppID_SpectraTRLow 0x0260

// Spectral zoom-in spectra; Priority:  High     
#define AppID_ZoomSpectra 0x0270

// Calibrator metadata packet; Priority:  High     
#define AppID_Calibrator_MetaData 0x0280

// Calibrator main payload (x3); Priority:  High     
#define AppID_Calibrator_Data 0x0281

// Calibrator raw PFB (8 packets for 4 x R/I); Priority:  High     
#define AppID_Calibrator_RawPFB 0x0284

// Calibrator debug data (8 packets); Priority:  High     
#define AppID_Calibrator_Debug 0x028C

// Very low priority, not expected to be normally downloaded; Priority:  VeryLow  
#define AppID_SpectraVeryLow 0x02D0

// x= 0...4 for 4 autocorrelatins; Priority:  High     
#define AppID_FW_DirectSpectrum 0x02E0

// x= 0..4 for 4 raw ADC data streams; Priority:  High     
#define AppID_RawADC 0x02F0



#endif