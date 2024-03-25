
#ifndef LUSEE_APPIDS_H
#define LUSEE_APPIDS_H

// No description available; Priority:  High     
#define AppID Read Response 0x0200

// No description available; Priority:  High     
#define AppID_Reset_Request 0x0201

// No description available; Priority:  High     
#define AppID_Registers_RB 0x0205

// No description available; Priority:  High     
#define AppID_uC_Housekeeping 0x0206

// No description available; Priority:  High     
#define AppID_Calibrator_Detect 0x0207

// No description available; Priority:  High     
#define AppID_uC_Startup 0x0208

// No description available; Priority:  High     
#define AppID_uC_Sequencer_complete 0x0209

// x = 0..F for 16 correlations; Priority:  High     
#define AppID_MetaData 0x020F

// No description available; Priority:  High     
#define AppID_SpectraHigh 0x0210

// No description available; Priority:  Med      
#define AppID_SpectraMed 0x0220

// No description available; Priority:  Low      
#define AppID_SpectraLow 0x0230

// No description available; Priority:  High     
#define AppID_SpectraRejectHigh 0x0240

// No description available; Priority:  Med      
#define AppID_SpectraRejectMed 0x0250

// No description available; Priority:  Low      
#define AppID_SpectraRejectLow 0x0260

// No description available; Priority:  High     
#define AppID_ZoomSpectra 0x0270

// No description available; Priority:  High     
#define AppID_TimeZoomSpectra 0x0280

// Very low priority, not expected to be normally downloaded; Priority:  VeryLow  
#define AppID_SpectraVeryLow 0x02D0

// x= 0...4 for 4 autocorrelatins; Priority:  High     
#define AppID_FW_DirectSpectrum 0x02E0

// x= 0..4 for 4 raw ADC data streams; Priority:  High     
#define AppID_RawADC 0x02F0



#endif