## flash_interface.c

This file manages flash memory operations for state persistence and calibration data:

1. `get_flash_addr` - Helper to calculate flash address for state slots
2. `get_cal_flash_addr` - Helper for calibration data addresses
3. `flash_state_store` - Saves current state to flash
4. `flash_slot_busy` - Checks if a slot is occupied
5. `flash_state_restore` - Restores state from flash
6. `clear_current_slot` - Erases current flash slot


7. `store_state` - Manages state saving across multiple slots
8. `restore_state` - Handles state restoration with bootloader compatibility
9. `flash_calweights_store` - Writes calibration weights to persistent storage
10. `flash_calweights_restore` - Retrieves calibration weights from flash
11. `region_get_info` - Extracts metadata for specific flash regions
12. `region_set_info` - Configures flash region metadata
13. `flash_region_enable` - Toggles access to flash memory regions
14. `region_check_checksum` - Verifies data integrity for flash regions
15. `region_copy_region` - Duplicates flash memory content
16. `flash_send_region_info` - Transmits region status through telemetry channels
17. `flash_copy_region_cmd` - Initiates region copying with comprehensive reporting

### `get_flash_addr(uint32_t slot)`
Helper function that calculates the flash memory address for a given state storage slot by multiplying the slot number by 4096 bytes and adding the Flash_FS_Save base address.

### `get_cal_flash_addr(uint32_t slot)`
Helper function that calculates the flash memory address for calibration weight storage by multiplying the slot number by 4096 bytes and adding the Flash_CAL_Save base address.

### `flash_state_store(uint8_t slot, struct core_state* state)`
Stores the current command sequence state to flash memory. Copies the command buffers (cmd_arg_high, cmd_arg_low), command pointers (cmd_ptr, cmd_end), and calculates a CRC checksum for integrity verification. The data is first assembled in the FLASH_WORK buffer with a magic number (0xBEBEC) to indicate the slot is in use, then written to the calculated flash address. This function is called when the spectrometer is off to avoid interference.

### `flash_slot_busy(uint8_t slot)`
Checks if a flash storage slot is currently occupied by reading the first 8 bytes and checking for the magic number 0xBEBEC. Returns true if the slot contains valid data, false if empty or corrupted.

### `flash_state_restore(uint8_t slot, struct core_state* state)`
Attempts to restore a previously saved state from the specified flash slot. First checks if the slot is busy, then reads the full saved_state structure and verifies the CRC checksum. If valid, copies the command buffers and pointers back into the current state. Returns true on successful restoration, false if the slot is empty or the CRC check fails (which also sets the FLASH_CRC_FAIL error flag).

### `clear_current_slot(struct core_state* state)`
Erases the currently active flash slot if one exists (flash_slot != -1). Performs a 4KB sector erase operation and resets the flash_slot to -1. Includes debug output showing which slot was cleared.

### `store_state(struct core_state* state)`
High-level function to save the current state to a randomly selected flash slot. First clears any existing slot, then selects a new slot based on the current timer value modulo MAX_STATE_SLOTS for pseudo-random distribution. Calls flash_state_store to perform the actual save operation and updates the state's flash_slot field.

### `restore_state(struct core_state* state)`
High-level state restoration function that handles different boot scenarios based on the value in uC register 0. If arg1==1 (ISS mode), performs a clean start. If arg1==2, erases all occupied flash slots. Otherwise, searches through all slots to find and restore the first valid saved state. After successful restoration, sends a CDI notification packet containing the slot number and sequence size. This function is typically called during system initialization to recover from resets.

### `flash_calweights_store(uint8_t slot)`
Saves all 512 calibration weights to flash memory. Reads weights from the calibrator hardware interface, calculates a CRC checksum, marks the slot with magic number 0xBABAC, and writes the complete structure to the calibration flash area.

### `flash_calweights_restore(uint8_t slot, bool just_check)`
Restores calibration weights from flash or just verifies their integrity. Reads the saved weights structure, verifies the magic number and CRC checksum. If just_check is false and validation passes, writes all 512 weights back to the calibrator hardware. Returns true if weights are valid, false otherwise. Includes debug output indicating success or failure.

### `region_get_info(uint32_t region, bool *valid, uint32_t *size, uint32_t *checksum)`
Retrieves metadata for a flash region (1-6). Reads 12 bytes from the region's metadata offset containing validity flag, size, and checksum. Sets valid=false for invalid region numbers. The validity is determined by checking if the first word equals the region number.

### `region_set_info(uint32_t region, uint32_t size, uint32_t checksum, bool enable)`
Updates metadata for a flash region. Writes the region number (or 0 to disable), size, and checksum to the region's metadata area. Ignores invalid region numbers (outside 1-6 range).

### `flash_region_enable(int region, bool enable)`
Enables or disables a flash region by updating its validity flag. Only modifies the metadata if the current state differs from the requested state to avoid unnecessary flash writes.

### `region_check_checksum(uint32_t region, bool* valid, uint32_t *size, uint32_t* info_checksum, uint32_t* data_checksum)`
Comprehensive validation of a flash region's data integrity. Retrieves region metadata, validates size bounds (non-zero and within Flash_Meta_Offset/4), reads the actual data from flash, and calculates a checksum using two's complement (~sum + 1). Returns both the stored checksum (info_checksum) and calculated checksum (data_checksum) for comparison. Sets valid=false if region is invalid, size is out of bounds, or if the region number is outside the 1-6 range.

### `region_copy_region(int region_src, int region_tgt, struct flash_copy_report_t *report)`
Copies data from one flash region to another with comprehensive error checking and reporting. Validates both regions are in range 1-6, verifies source region checksum matches stored metadata, performs the copy operation, updates target region metadata, and verifies the copy succeeded by checking the target checksum. The report structure is populated with status codes (FLASH_BAD_REGIONS, FLASH_COPY_BAD_CHECKSUM_IN, FLASH_COPY_BAD_CHECKSUM_OUT, or FLASH_COPY_SUCCESS) and relevant checksums for diagnostic purposes. Debug output tracks the operation progress.

### `flash_send_region_info(struct core_state *state)`
Sends a housekeeping packet (type 100) containing flash region status information via CDI. For all 6 regions, performs checksum validation and includes validity flags, sizes, and both stored and calculated checksums in the telemetry packet. Updates timestamps and packet IDs before transmission.

### `flash_copy_region_cmd(struct core_state *state, int region_src, int region_tgt)`
Executes a flash region copy command and reports results via CDI housekeeping packet (type 101). Calls region_copy_region to perform the actual copy, then sends a detailed report including the operation status and all relevant checksums through the telemetry system.
