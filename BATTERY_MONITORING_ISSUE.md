# Battery Monitoring System Implementation

## 🔋 Issue Summary
Implement comprehensive battery monitoring functionality for the Lightning Detection System to enable power-aware operation, battery health tracking, and intelligent power management on Heltec WiFi LoRa 32 V3 and Wireless Tracker boards.

## 📋 Description
The current Hardware Abstraction Layer (HAL) provides basic battery monitoring stubs (`Power::getBatteryVoltage()` and `Power::getBatteryPercent()`), but lacks a complete implementation. This feature is critical for:

- **Field deployment reliability** - Monitor battery status during remote operation
- **Power optimization** - Adjust transmission frequency based on battery level
- **Predictive maintenance** - Alert users before battery depletion
- **Safety compliance** - Prevent deep discharge damage to batteries

## 🎯 Goals
1. **Accurate voltage measurement** using ESP32-S3 ADC with calibration
2. **Battery percentage calculation** with configurable battery chemistry profiles
3. **Health monitoring** - Track charging cycles and capacity degradation
4. **Power alerts** - Low battery warnings and critical shutdown thresholds
5. **Data logging** - Historical battery performance tracking
6. **Integration** - Seamless operation with existing LoRa messaging and OLED display

## 🔧 Technical Requirements

### Hardware Integration
- **ADC Channel**: Use available ADC pin for voltage divider measurement
- **Voltage Divider**: External resistor network for safe voltage measurement
- **Calibration**: Account for ESP32-S3 ADC non-linearity and reference voltage variations
- **Temperature Compensation**: Adjust readings based on operating temperature

### Software Architecture
```cpp
namespace Power {
    // Battery configuration
    struct BatteryConfig {
        float voltage_min;      // Minimum safe voltage (e.g., 3.0V for Li-Ion)
        float voltage_max;      // Maximum voltage (e.g., 4.2V for Li-Ion)
        uint16_t capacity_mah;  // Battery capacity in mAh
        float divider_ratio;    // Voltage divider ratio
        uint8_t adc_pin;        // ADC pin for measurement
        BatteryChemistry type;  // Li-Ion, LiFePO4, etc.
    };

    // Battery status information
    struct BatteryStatus {
        float voltage;          // Current voltage (V)
        uint8_t percentage;     // Charge percentage (0-100%)
        int16_t current_ma;     // Current draw (mA, negative = discharging)
        float temperature;      // Battery temperature (°C)
        BatteryHealth health;   // GOOD, DEGRADED, POOR, CRITICAL
        uint32_t cycles;        // Charging cycles count
        bool is_charging;       // Charging state
        uint32_t time_remaining; // Estimated runtime (seconds)
    };

    // Battery chemistry profiles
    enum class BatteryChemistry {
        LI_ION,           // Standard Li-Ion (3.0V - 4.2V)
        LI_PO,            // Li-Polymer (3.0V - 4.2V)  
        LIFEPO4,          // LiFePO4 (2.5V - 3.6V)
        ALKALINE,         // Alkaline (2.4V - 3.2V)
        CUSTOM            // User-defined profile
    };

    // Implementation functions
    Result initializeBatteryMonitoring(const BatteryConfig& config);
    BatteryStatus getBatteryStatus();
    Result setBatteryConfig(const BatteryConfig& config);
    Result calibrateBatteryMonitor();
    Result logBatteryData();
}
```

### Measurement Strategy
1. **Multi-sample averaging** - Reduce ADC noise through oversampling
2. **Filtering** - Low-pass filter for stable readings
3. **Calibration lookup table** - Compensate for ADC non-linearity
4. **Temperature correction** - Adjust for thermal effects on battery voltage
5. **Load compensation** - Account for voltage drop under different loads

## 📊 Implementation Plan

### Phase 1: Basic Voltage Measurement
- [ ] **ADC Configuration** - Set up dedicated ADC channel with proper resolution
- [ ] **Voltage Divider** - Design and implement external voltage sensing circuit
- [ ] **Calibration** - Create calibration routine for accurate voltage measurement
- [ ] **Basic API** - Implement `getBatteryVoltage()` with real measurements

### Phase 2: Percentage Calculation  
- [ ] **Battery Profiles** - Implement discharge curves for different chemistries
- [ ] **Percentage Algorithm** - Convert voltage to percentage using lookup tables
- [ ] **State of Charge** - Implement coulomb counting for improved accuracy
- [ ] **API Enhancement** - Complete `getBatteryPercent()` implementation

### Phase 3: Advanced Monitoring
- [ ] **Health Tracking** - Monitor capacity degradation over time
- [ ] **Cycle Counting** - Track charging/discharging cycles
- [ ] **Temperature Monitoring** - Integrate temperature sensor data
- [ ] **Predictive Analytics** - Estimate remaining runtime based on usage patterns

### Phase 4: System Integration
- [ ] **LoRa Integration** - Include battery data in transmitted messages
- [ ] **OLED Display** - Show battery status on screen with icons/graphs
- [ ] **Power Management** - Implement power-saving modes based on battery level
- [ ] **Alerts System** - Generate warnings for low battery conditions

## 🧪 Testing Requirements

### Unit Tests
```cpp
// Test basic voltage measurement
void test_battery_voltage_reading();
void test_battery_percentage_calculation();
void test_battery_calibration();
void test_battery_chemistry_profiles();

// Test advanced features
void test_battery_health_monitoring();
void test_battery_cycle_counting();
void test_battery_temperature_compensation();

// Test integration
void test_battery_lora_integration();
void test_battery_oled_display();
void test_battery_power_management();
```

### Hardware Tests
- **Voltage accuracy** - Verify ±50mV accuracy across operating range
- **Temperature stability** - Test across -20°C to +60°C range
- **Load variation** - Validate readings under different current draws
- **Long-term stability** - Monitor accuracy over extended periods

## 🔌 Hardware Considerations

### Heltec WiFi LoRa 32 V3
- **Available ADC pins**: GPIO1-10 (ADC1), GPIO11-20 (ADC2)
- **ADC resolution**: 12-bit (4096 levels)
- **Reference voltage**: Internal 1.1V reference
- **Input range**: 0-3.3V (with voltage divider for higher voltages)

### Wireless Tracker
- **Battery connector**: JST connector for Li-Ion battery
- **Charging circuit**: Integrated charging IC
- **Power monitoring**: Dedicated battery monitoring circuit
- **GPIO availability**: Limited pins due to GPS integration

### External Components
```
Battery Monitor Circuit:
VBat ----[R1]----+----[R2]----GND
                 |
                ADC_PIN

Where: R1/(R1+R2) = desired voltage division ratio
Example: For 4.2V max → 3.1V ADC max
R1 = 10kΩ, R2 = 27kΩ (ratio ≈ 0.73)
```

## 📋 Acceptance Criteria

### Functional Requirements
- [ ] **Voltage accuracy**: ±50mV across 2.5V-4.5V range
- [ ] **Percentage accuracy**: ±5% for charge estimation  
- [ ] **Update rate**: Configurable 1-60 second intervals
- [ ] **Low power**: <1mA additional current consumption
- [ ] **Temperature range**: Operational -20°C to +60°C

### Performance Requirements
- [ ] **Response time**: <100ms for status query
- [ ] **Memory usage**: <2KB additional RAM
- [ ] **Flash usage**: <8KB additional program memory
- [ ] **Calibration**: Self-calibration capability

### Integration Requirements
- [ ] **HAL compatibility**: Seamless integration with existing HAL
- [ ] **Unit tests**: 100% test coverage for battery monitoring functions
- [ ] **Documentation**: Complete API documentation and usage examples
- [ ] **Backward compatibility**: No breaking changes to existing code

## 🔗 Dependencies

### Hardware Dependencies
- **ADC Channel**: Available GPIO pin with ADC capability
- **External components**: Voltage divider resistors, bypass capacitors
- **Battery connection**: Proper battery connector and wiring

### Software Dependencies  
- **HAL Framework**: Existing Hardware Abstraction Layer
- **ADC Driver**: ESP32-S3 ADC configuration and calibration
- **NVS Storage**: For storing calibration data and battery history
- **Timer System**: For periodic battery monitoring

### Related Issues
- **Hardware Abstraction Layer** - #7 ✅ (Completed - provides foundation)
- **Modular Architecture** - #14 🚧 (In Progress - defines integration patterns)
- **Power Management** - (Future issue for sleep mode optimization)

## 🎯 Success Metrics

### Technical Metrics
- **Measurement accuracy**: Achieve ±2% voltage accuracy after calibration
- **Battery life prediction**: ±10% accuracy for remaining runtime estimation
- **System reliability**: 99.9% uptime for battery monitoring function
- **Power efficiency**: <0.1% impact on overall system power consumption

### User Experience Metrics
- **Display clarity**: Clear battery status indication on OLED
- **Alert effectiveness**: Timely low-battery warnings (>30 minutes notice)
- **Data quality**: Consistent and reliable battery data in LoRa messages
- **Maintenance ease**: Simple calibration process for field deployment

## 📚 Documentation Requirements

### Technical Documentation
- **API Reference**: Complete function documentation with examples
- **Hardware Guide**: Circuit diagrams and component specifications  
- **Calibration Procedure**: Step-by-step calibration instructions
- **Troubleshooting Guide**: Common issues and solutions

### User Documentation
- **Setup Instructions**: Battery monitoring configuration guide
- **Operating Manual**: How to interpret battery status information
- **Maintenance Guide**: Battery care and replacement procedures
- **Performance Tuning**: Optimization tips for different use cases

## 🏷️ Labels
- `enhancement` - New feature implementation
- `hardware` - Hardware integration required
- `hal` - Hardware Abstraction Layer component
- `power-management` - Power system related
- `high-priority` - Important for field deployment
- `testing-required` - Comprehensive testing needed

## 📅 Timeline Estimate
- **Phase 1**: 2-3 weeks (Basic voltage measurement)
- **Phase 2**: 2-3 weeks (Percentage calculation and profiles)
- **Phase 3**: 3-4 weeks (Advanced monitoring features)  
- **Phase 4**: 2-3 weeks (System integration and polish)
- **Total**: ~10-13 weeks for complete implementation

## 💡 Additional Notes

### Future Enhancements
- **Wireless charging detection** - Detect when device is on charging pad
- **Battery authentication** - Verify genuine battery packs
- **Multi-battery support** - Support for systems with multiple batteries
- **Cloud integration** - Upload battery health data for fleet management

### Considerations
- **Safety first** - Implement proper safeguards against overcharge/discharge
- **Certification** - Consider battery monitoring requirements for regulatory compliance
- **Environmental** - Account for extreme weather conditions in field deployment
- **Maintenance** - Design for easy battery replacement and calibration

---

**This issue is part of the modular architecture milestone and builds upon the completed Hardware Abstraction Layer foundation.**