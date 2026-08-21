# MagCal

**Non-contact magnetic thickness measurement system using a PIC24 microcontroller, Hall-effect sensing, custom PCB hardware, embedded C firmware, and calibration-based signal processing.**

MagCal was developed as a **CSCI 399 Independent Study** project at Hunter College.

The goal of the project is to estimate material thickness from changes in magnetic field strength rather than through a conventional mechanical caliper. The system measures the field produced by a controlled magnet/sensor geometry, processes the sensor output using the PIC24 ADC, applies calibration data, and displays the resulting thickness estimate on an LCD.

---

## Project Overview

MagCal combines:

- Embedded C firmware
- PIC24 microcontroller development
- Hall-effect sensing
- High-resolution ADC acquisition
- Custom PCB design
- Mechanical design
- Calibration and signal processing
- Drift and repeatability testing
- Low-voltage monitoring
- LCD-based user feedback

The complete project includes the firmware, KiCad hardware files, mechanical designs, production files, datasheets, and calibration datasets.

---

## Prototype

<p align="center">
  <img src="/docs/images/full_system.jpg" width="700">
</p>

<p align="center">
  <em>Complete MagCal prototype showing the sensing probe, magnetic target, control electronics, LCD interface, and battery supply.</em>
</p>

## How It Works

MagCal measures material thickness using a **permanent magnet** on one side of the material and a **Hall-effect sensor** on the other.

The Hall sensor measures the magnetic field, which changes with the distance between the sensor and magnet. The **PIC24FJ128GC010** samples this signal using its internal ADC and averages **500 samples** to reduce noise.

The reading is normalized using the **far and close calibration points** and corrected for variations in the calibration span. The firmware then selects an experimentally generated calibration table and maps the reading to a thickness value displayed on the LCD.

<p align="center">
  <img src="/docs/images/demo.jpg" width="700">
</p>

<p align="center">
  <em>Measuring an acoustic guitar soundboard with the magnet inside the instrument and the sensing probe outside.</em>
</p>

### Lab / Testing Setup

<p align="center">
  <img src="docs/images/development.jpg" width="700">
</p>

<p align="center">
  <em>Prototype development, debugging, and calibration work.</em>
</p>

## System Architecture

```text
      Permanent Magnet
             │
             │  Magnetic Field
             ▼
     Hall-Effect Sensor
             │
             │  Analog Voltage
             ▼
        PIC24FJ MCU
             │
        ┌────┴────┐
        │   ADC   │
        └────┬────┘
             │
      500-Sample Average
             │
      Baseline Correction
             │
      Gain Normalization
             │
     Calibration Lookup
             │
             ▼
      Thickness Estimate
             │
             ▼
            LCD
```

The firmware acquires sensor readings, averages multiple ADC samples, normalizes the measurement using close and far calibration baselines, selects the appropriate calibration dataset, and converts the normalized reading into a thickness estimate.

---

## Hardware

The electronics were designed as a custom PCB using **KiCad**.

The system includes:

- PIC24FJ microcontroller
- Hall-effect magnetic sensor
- Analog sensor interface
- LCD
- Status LEDs
- Calibration inputs
- Low-voltage detection
- Supporting power and passive circuitry

The PCB schematic, layout, Gerbers, and production outputs are available in:

```text
Hardware/
```

### PCB Render

> Add a front-side KiCad 3D render here.

```text
docs/images/pcb_front.png
```

```markdown
![PCB front render](docs/images/pcb_front.png)
```

### PCB Back

> Optional second render.

```text
docs/images/pcb_back.png
```

```markdown
![PCB back render](docs/images/pcb_back.png)
```

---

## Firmware

The firmware is written in **embedded C** for the PIC24 platform.

Major responsibilities include:

- GPIO initialization
- ADC initialization and acquisition
- 500-sample averaging
- Close-baseline calibration
- Automatic far-baseline calibration
- Measurement normalization
- Gain correction
- Calibration-table selection
- Thickness lookup
- LCD output
- Low-voltage detection
- Watchdog servicing
- User feedback through status LEDs

The main measurement loop follows this sequence:

```text
Acquire averaged ADC reading
        ↓
Validate calibration state
        ↓
Normalize against far/close baselines
        ↓
Apply gain correction
        ↓
Select calibration table
        ↓
Map normalized value to thickness
        ↓
Display result
        ↓
Service watchdog
```

Firmware source is available under:

```text
Firmware/
```

---

## Measurement Averaging

Individual ADC samples contain enough variation to affect fine measurements.

Instead of using a single sample, MagCal averages **500 ADC readings** per measurement cycle.

```c
short int getAveragedReadings(void)
{
    long sum = 0;

    for (int i = 0; i < SAMPLING; i++)
    {
        sum += ADC16_read();
        __delay_ms(1);
    }

    return (short int)(sum / SAMPLING);
}
```

This reduces short-term measurement noise before calibration and thickness mapping are applied.

---

## Calibration System

Magnetic field strength does not vary linearly with distance, so a simple linear conversion between ADC reading and thickness is not sufficient.

MagCal therefore uses experimentally derived calibration data.

Each calibration point contains:

```c
typedef struct
{
    float threshold;
    float thickness;
} CalibrationPoint;
```

Calibration values are stored as lookup tables rather than large hard-coded conditional chains.

Example:

```c
static const CalibrationPoint calibration_990[] =
{
    {0.991f, 0.00f},
    {0.980f, 0.25f},
    {0.955f, 0.50f},
    {0.921f, 0.75f},
    {0.910f, 1.00f},
    ...
};
```

A generic lookup function then converts the normalized sensor reading into a thickness value:

```c
float mapReading(float reading,
                 const CalibrationPoint *table,
                 unsigned int size,
                 float maximum_thickness)
{
    for (unsigned int i = 0; i < size; i++)
    {
        if (reading >= table[i].threshold)
            return table[i].thickness;
    }

    return maximum_thickness;
}
```

This separates **calibration data** from the **mapping algorithm**, making the firmware easier to inspect, modify, and validate.

Calibration datasets are stored under:

```text
data/
```

---

## Baseline Calibration

The system uses two reference measurements:

- **Far baseline**
- **Close baseline**

These values define the working measurement span.

```c
normalized_result =
    (float)(result - baselineFar) /
    (float)(baselineClose - baselineFar);
```

The calculated span is then compared against the nominal calibration span to compensate for variation between calibration cycles.

```c
gain_correction =
    (baselineClose - baselineFar) /
    (float)CAL_SPAN;

normalized_result *= gain_correction;
```

The firmware also protects against a zero baseline span before performing normalization.

---

## Automatic Far-Baseline Detection

The firmware can automatically trigger far-baseline calibration when the measured ADC value enters the expected range.

```c
#define FAR_BASELINE_CALIBRATE_TRIGGER_THRESHOLD 10000
#define FAR_BASELINE_MIN 6400
#define FAR_BASELINE_MAX 7000
```

A candidate far-baseline measurement is accepted only if all sampled readings remain within the permitted range.

This helps reject invalid calibration attempts caused by incorrect magnet position or unexpected sensor values.

---

## Close Calibration

Close-baseline calibration is triggered by the calibration input:

```c
#define CLOSE_CALIBRATION_REQUESTED() (PORTEbits.RE7 == 1)
```

When calibration completes successfully, the device gives visual feedback through the status LEDs.

---

## ADC Debugging and MCU Errata

One of the most significant debugging challenges during the development of MagCal involved the PIC24FJ128GC010 ADC.

During early testing, the ADC produced unexpected readings that initially appeared to be caused by the Hall sensor, analog circuitry, or firmware configuration. Debugging therefore involved testing the measurement chain step by step and comparing the observed ADC behavior with the PIC24FJ128GC010 documentation and silicon errata.

The issue was eventually traced to documented device-specific ADC behavior rather than to the Hall sensor itself.

### Initial ADC Implementation

The first implementation used the PIC24's pipeline ADC. During testing, abnormal ADC behavior was observed over part of the expected input range.

This was particularly problematic for MagCal because the measurement depends on relatively small changes in the Hall sensor output. An ADC error that might be acceptable in another application could directly translate into an incorrect thickness measurement.

The problem was investigated by:

- Testing known and controlled ADC input conditions
- Comparing raw ADC values against the expected sensor behavior
- Checking the analog input configuration
- Verifying ADC initialization and reference configuration
- Reviewing the PIC24FJ128GC010 datasheet and silicon errata
- Separating sensor behavior from ADC behavior during testing

The investigation showed that the unexpected measurements were related to documented ADC limitations/errata of the microcontroller.

---

### Moving to the 16-bit Sigma-Delta ADC

To improve the measurement path, the design was moved to the PIC24FJ128GC010's **16-bit Sigma-Delta ADC** using a differential measurement configuration.

The firmware was updated accordingly, including the ADC initialization sequence and readiness/calibration handling required by the peripheral.

This provided a more suitable acquisition path for the low-frequency precision measurement performed by MagCal.

The Hall sensor signal is sampled repeatedly and the resulting ADC measurements are averaged before being used by the calibration algorithm.

---

### ADC Reference / REFPUMP Issue

Another important issue involved the ADC reference circuitry.

During debugging, incorrect or unstable ADC behavior was traced to the reference configuration required by the Sigma-Delta ADC. The PIC24FJ128GC010 documentation and errata were used to determine the required hardware configuration.

A hardware modification was therefore made to the existing PCB rather than manufacturing a completely new revision of the board.

A small jumper was added to implement the required reference connection.

### Hardware Modification

<p align="center">
  <img src="docs/images/eratta.jpg" width="600">
</p>

<p align="center">
  <em>Hardware modification implementing the PIC24FJ128GC010 ADC errata workaround.</em>
</p>

This modification is intentionally visible in the project documentation because it represents an important part of the engineering process: identifying a silicon/device-specific problem, finding the relevant information in the manufacturer's documentation, and implementing a hardware-level correction on an already fabricated PCB.

---

### ADC Initialization

The ADC firmware also had to account for the peripheral's required initialization and calibration sequence.

In particular, ADC operation could not simply begin immediately after enabling the peripheral. The firmware had to wait for the appropriate ADC ready/calibration state before measurements could be considered valid.

This was incorporated into the ADC initialization code in:

```text
Firmware/ADC.c
```

---

### Debugging Outcome

Resolving the ADC problems required debugging across several layers of the system:

```text
Hall Sensor
     │
     ▼
Analog Signal
     │
     ▼
PCB / Reference Circuitry
     │
     ▼
PIC24 ADC Hardware
     │
     ▼
ADC Configuration
     │
     ▼
Raw ADC Samples
     │
     ▼
Averaging / Calibration
     │
     ▼
Thickness Measurement
```

Rather than compensating for the unexpected readings in software, the underlying ADC and reference problems were investigated and corrected.

This debugging process was one of the most important parts of the project because it required working across **firmware, analog electronics, PCB hardware, microcontroller documentation, and silicon errata** rather than treating each area independently.

## Sensor Drift

Hall-effect measurements change slightly while the electronics reach thermal equilibrium.

The system was experimentally characterized for:

- Warm-up behavior
- Baseline drift
- Measurement repeatability
- Sensor-to-magnet positioning sensitivity

These observations influenced both the calibration process and the firmware design.

### Drift / Stability Graph

> Add a graph showing baseline drift or repeated measurements over time.

```text
docs/images/drift_plot.png
```

```markdown
![Baseline drift measurement](docs/images/drift_plot.png)
```

---

## Calibration Results

> Add one representative calibration graph here.

A useful plot would show:

```text
Normalized sensor reading
        vs
Known thickness
```

Suggested file:

```text
docs/images/calibration_curve.png
```

```markdown
![Calibration curve](docs/images/calibration_curve.png)
```

Avoid presenting a final accuracy or resolution claim here unless it is supported by validated measurements.

---

## Mechanical Design

Because magnetic field strength depends strongly on geometry, mechanical repeatability is important to measurement consistency.

Custom mechanical components were designed to maintain repeatable positioning between:

- Hall sensor
- Magnet
- Measured material

Mechanical designs and printable models are available in:

```text
Mechanical/
```

### Mechanical Render

> Add a FreeCAD or STL render here if useful.

```text
docs/images/mechanical_render.png
```

```markdown
![Mechanical assembly](docs/images/mechanical_render.png)
```

---

## Low-Voltage Protection

The system uses the PIC24 high/low-voltage detection functionality to detect insufficient battery voltage.

When low voltage is detected, the firmware disables further HLVD interrupts, informs the user through the LCD, and remains in a safe watchdog-serviced state.

```c
void __attribute__((interrupt, no_auto_psv)) _LVDInterrupt(void)
{
    IEC4bits.HLVDIE = 0;
    IFS4bits.HLVDIF = 0;

    LCD_print_string("Change battery", 0);

    while (1)
    {
        ClrWdt();
    }
}
```

---

## Repository Structure

```text
MagCal/
├── Firmware/        PIC24 embedded C firmware
├── Hardware/        KiCad schematic, PCB and production files
├── Mechanical/      Mechanical CAD and printable models
├── data/            Calibration and measurement datasets
├── Datasheets/      Component documentation
├── Libs/            Supporting design libraries
├── docs/
│   └── images/      README photos, renders and plots
└── README.md
```

---

## Tools and Technologies

### Firmware

- Embedded C
- PIC24FJ
- MPLAB X
- XC16
- ADC acquisition
- Watchdog timer
- HLVD interrupt handling
- LCD interfacing

### Hardware

- KiCad
- PCB design
- PCB fabrication
- Hall-effect sensing
- Analog measurement
- Component-level debugging
- Soldering and PCB rework

### Mechanical / Experimental

- FreeCAD
- 3D printing
- Sensor characterization
- Calibration
- Repeatability testing
- Drift analysis

---

## Engineering Takeaways

This project involved considerably more than reading a sensor and displaying a value.

Some of the main engineering lessons included:

- Debugging hardware and firmware as a single system
- Reading and applying microcontroller errata
- Characterizing analog sensor behavior experimentally
- Designing calibration around real-world sensor variation
- Separating calibration data from firmware logic
- Reducing measurement noise through sampling and averaging
- Designing mechanical geometry for repeatable measurements
- Producing custom PCB hardware alongside firmware
- Building fault handling into an embedded system

---

## Project Status

MagCal has progressed through:

- Hardware design
- PCB fabrication
- Firmware development
- Sensor integration
- ADC debugging
- Mechanical prototyping
- Calibration
- Drift characterization
- Measurement testing

The current firmware uses multiple calibration tables selected according to baseline conditions.

Further work may include:

- Additional calibration refinement
- Temperature/drift compensation
- Improved mechanical repeatability
- Further validation of measurement accuracy and resolution

---

## Author

**Theodoros Bogiatzis**  
Computer Science  
Hunter College, CUNY
