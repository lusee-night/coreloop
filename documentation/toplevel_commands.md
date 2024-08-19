
# Top-level CDI commands

These are top-level command, the actual CDI commands. Majority of the spectrometer commanding goes under `RFS_Settings` described in [this table](lusee_commands.md).
Commands above 0xA0 are parsed internally by the test framework and not described in the official ICD


|  Hex | Name                |  Description                                       |
|------|---------------------|----------------------------------------------------|                             
| 0x10 | RFS_SETTINGS        | Master command for uC control                                      | 
| 0xA0 | CTRL_OUTLIER_NUM    | Generate outliers in this number                                   |
| 0xA1 | CTRL_OUTLIER_AMP    | Generate outliers of this amplitude in units of 1/256              |
| 0xA2 | CTRL_OUTLIER_BINS   | Generate outliers of this many (sequential) bins                   |
| 0xE0 | CTRL_WAIT           | Wait for 0.1s x argument (intercepted by commander)                | 