## Changes in sdkconfig (menuconfig)
>[!NOTE]
> After making changes, use idf.py save-defconfig to regenerate the sdkconfig.defaults file.
### Enhance performance

* Bootloader config
  * Bootloader optimization Level
    * **Optimize for performance (-O2)**
* Compiler options
  * Optimization Level
    * **Optimize for performance (-O2)**

### Reduce heap footprint

* Compiler options
  * Assertion level
    * **Disabled (sets -DNDEBUG)**
* Component config
  * LWIP
    * [ ] **Enable IPv6**
* Component config
  * LWIP
    * Enable PPP support
      * [ ] **Enable PPP support**
* Component config
  * LWIP
    * esp-modem
      * [ ] **Use PPP mode**
* Component config
  * Diagnostics
    * [x] **Use external log wrapper**
* Log 
  * Log Level
    * Default log verbosity
      * **No output**

### For ratpak exceptions

* Compiler options
  * Enable C++ exceptions
    * [x] **Enable C++ exceptions**

### Keep watchdog happy during long calculations
* ESP System Settings
  * Task Watchdog timeout period (seconds)
    * **10**

### Modbus stack support for TCP currently does not compile if IPv6 is disabled
* Component config
  * Modbus configuration
    * [ ] **Enable Modbus stack support for TCP communication mode**

### For OTA
* Serial flasher config
  * Flash size
    * 16 MB
   
* Partition Table
  * Partition Table
    * Custom partition table CSV
  * Custom partition CSV file
    * partitions.csv



