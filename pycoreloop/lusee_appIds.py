
          
# This file is generated by scripts/create_appid_include.py
# It includes all the appIds that are available
# generated from the documentation/lusee_appIds.md file.

# AppIds
# RFS register read back; Priority:  High     
AppID_Read_Response = 0x0200

# RFS watchdog is requesting a reset + metadata; Priority:  High     
AppID_Reset_Request = 0x0201

# Dumps all register values from the RFS into packet; Priority:  High     
AppID_Registers_RB = 0x0205

# uC generater housekeeping; Priority:  High     
AppID_uC_Housekeeping = 0x0206

# Calibrator detected with meta data; Priority:  High     
AppID_Calibrator_Detect = 0x0207

# Data from the bootloader; Priority:  High     
AppID_uC_Bootloader = 0x0208

# Flight SW has booted; Priority:  High     
AppID_uC_Start = 0x0209

# Regular hearbeat packet; Priority:  NONE     
AppID_uC_Heartbeat = 0x020A

# For sequencer with a limited number of steps; Priority:  High     
AppID_uC_Sequencer_complete = 0x020B

# x = 0..F for 16 correlations; Priority:  High     
AppID_MetaData = 0x020F

# Main correlation products, high priorty; Priority:  High     
AppID_SpectraHigh = 0x0210

# Main correlation products, medium priority; Priority:  Med      
AppID_SpectraMed = 0x0220

# Main correlation products, low priority; Priority:  Low      
AppID_SpectraLow = 0x0230

# Time-resolved spectra, high priority; Priority:  High     
AppID_SpectraTRHigh = 0x0240

# Time-resolved spectra, medium priority; Priority:  Med      
AppID_SpectraTRMed = 0x0250

# Time-resolved Spectra, low priority; Priority:  Low      
AppID_SpectraTRLow = 0x0260

# Spectral zoom-in spectra; Priority:  High     
AppID_ZoomSpectra = 0x0270

# Calibrator data; Priority:  High     
AppID_Calibrator_Data = 0x0280

# Very low priority, not expected to be normally downloaded; Priority:  VeryLow  
AppID_SpectraVeryLow = 0x02D0

# x= 0...4 for 4 autocorrelatins; Priority:  High     
AppID_FW_DirectSpectrum = 0x02E0

# x= 0..4 for 4 raw ADC data streams; Priority:  High     
AppID_RawADC = 0x02F0
