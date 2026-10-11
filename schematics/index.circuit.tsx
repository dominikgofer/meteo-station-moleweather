// Portable weather station - schematic
// Source of truth: components-list.md + datasheets in components-documentation/
//
// Rails:   V5 (from 18650 charger/boost shield) -> ESP32 devkit 5V pin, PMS5003 VCC
//          V3V3 (ESP32 devkit regulator)        -> BME280, BH1750, DS18B20, microSD, pull-ups
// Buses:   I2C (BME280 + BH1750), SPI (microSD), UART1 (PMS5003), 1-Wire (DS18B20)
// Sleep:   Q1 (2N7000) low-side switches the PMS5003 ground (PMS_GND) from a GPIO

// ESP32-C6-DevKitC-1 (ESP32-C6-WROOM-1). Only the pins used by this design are drawn.
// GPIO choices avoid the strapping pins GPIO8/9/15 and MTMS/MTDI (GPIO4/5), and the
// USB D+/D- (GPIO12/13) and UART0 console (GPIO16/17) pins - see datasheet section 4.
const esp32Pins = {
  pin1: "V5",
  pin2: "V3V3",
  pin3: "GND",
  pin4: "GPIO0",
  pin5: "GPIO1",
  pin6: "GPIO2",
  pin7: "GPIO3",
  pin8: "GPIO6",
  pin9: "GPIO7",
  pin10: "GPIO10",
  pin11: "GPIO20",
  pin12: "GPIO21",
  pin13: "GPIO23",
} as const

// Waveshare "Micro SD Storage Board" header P1 (see MicroSD_Storage_Board_schematic.pdf):
// 1 CD, 2 D1, 3 D2, 4 D3, 5 CLK, 6 CMD, 7 D0, 8 GND, 9 3.3V. In SPI mode:
// D3 = CS, CMD = MOSI, CLK = SCK, D0 = MISO. 3.3 V only, 10k pull-ups on board.
const sdPins = {
  pin1: "CD",
  pin2: "D1",
  pin3: "D2",
  pin4: "CS",
  pin5: "SCK",
  pin6: "MOSI",
  pin7: "MISO",
  pin8: "GND",
  pin9: "V3V3",
} as const

// PMS5003 digital interface (PMS5003_datasheet.pdf): 5 V supply, 3.3 V logic.
const pmsPins = {
  pin1: "VCC",
  pin2: "GND",
  pin3: "SET",
  pin4: "RXD",
  pin5: "TXD",
  pin6: "RESET",
  pin7: "NC7",
  pin8: "NC8",
} as const

export default () => (
  <board routingDisabled pcbPack schLayout={{ layoutMode: "relative" }} schTraceAutoLabelEnabled>
    <schematicsheet name="Weather Station" displayName="Portable Weather Station" sheetIndex={0}>
    <schematictext text="Power (18650 + charger)" schX={-5.5} schY={2.2} fontSize={0.3} anchor="center" />
    <schematictext text="Main controller" schX={0} schY={-2.6} fontSize={0.3} anchor="center" />
    <schematictext text="MicroSD (SPI)" schX={-5} schY={-6.0} fontSize={0.3} anchor="center" />
    <schematictext text="PMS5003 + power switch" schX={7} schY={-6.0} fontSize={0.3} anchor="center" />
    <schematictext text="I2C sensors" schX={14} schY={-3.2} fontSize={0.3} anchor="center" />
    <schematictext text="1-Wire sensor" schX={14} schY={-7.2} fontSize={0.3} anchor="center" />

    {/* ---------------- Power ---------------- */}
    <battery
      name="BAT1"
      manufacturerPartNumber="18650 Li-Ion 3400mAh"
      capacity="3400mAh"
      voltage="3.7V"
      footprint="pinrow2"
      schX={-7}
      schY={4}
      schOrientation="vertical"
    />
    {/* 18650 charger / boost shield: Li-Ion in, regulated 5 V out. USB_IN is the charge port. */}
    <chip
      name="U2"
      manufacturerPartNumber="18650_CHARGER_SHIELD"
      schWidth={2.5}
      schHeight={1}
      footprint="pinrow6"
      pinLabels={{
        pin1: "BAT_POS",
        pin2: "BAT_NEG",
        pin3: "USB_IN",
        pin4: "GND_IN",
        pin5: "V5_OUT",
        pin6: "GND_OUT",
      }}
      schPinArrangement={{
        leftSide: { pins: ["BAT_POS", "BAT_NEG"], direction: "top-to-bottom" },
        rightSide: { pins: ["V5_OUT", "GND_OUT"], direction: "top-to-bottom" },
        bottomSide: { pins: ["USB_IN", "GND_IN"], direction: "left-to-right" },
      }}
      schX={-4}
      schY={4}
    />
    <trace from="BAT1.pin1" to="U2.BAT_POS" />
    <trace from="BAT1.pin2" to="U2.BAT_NEG" />
    <trace from="U2.V5_OUT" to="net.V5" />
    <trace from="U2.GND_OUT" to="net.GND" />
    <trace from="U2.BAT_NEG" to="net.GND" />
    <trace from="U2.GND_IN" to="net.GND" />

    {/* ---------------- MCU ---------------- */}
    <chip
      name="U1"
      manufacturerPartNumber="ESP32-C6-DevKitC-1-N8"
      footprint="pinrow13"
      pinLabels={esp32Pins}
      schPinArrangement={{
        topSide: { pins: ["V3V3"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: {
          pins: ["V5", "GPIO6", "GPIO7", "GPIO10", "GPIO2"],
          direction: "top-to-bottom",
        },
        rightSide: {
          pins: ["GPIO0", "GPIO1", "GPIO3", "GPIO20", "GPIO21", "GPIO23"],
          direction: "top-to-bottom",
        },
      }}
      pinAttributes={{
        V5: { requiresPower: true },
        V3V3: { providesPower: true },
        GND: { mustBeConnected: true },
      }}
      schX={0}
      schY={0}
    />
    <trace from="U1.V5" to="net.V5" />
    <trace from="U1.V3V3" to="net.V3V3" />
    <trace from="U1.GND" to="net.GND" />

    {/* ---------------- MicroSD (SPI) ---------------- */}
    <chip
      name="U7"
      manufacturerPartNumber="Waveshare 3947 (Micro SD Storage Board)"
      footprint="pinrow9"
      pinLabels={sdPins}
      schPinArrangement={{
        topSide: { pins: ["V3V3"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: {
          pins: ["CS", "SCK", "MOSI", "MISO"],
          direction: "top-to-bottom",
        },
        rightSide: { pins: ["CD", "D1", "D2"], direction: "top-to-bottom" },
      }}
      schX={-5}
      schY={-4}
    />
    <trace from="U7.V3V3" to="net.V3V3" />
    <trace from="U7.GND" to="net.GND" />
    <trace from="U7.CS" to="net.SD_CS" />
    <trace from="U7.SCK" to="net.SPI_SCK" />
    <trace from="U7.MOSI" to="net.SPI_MOSI" />
    <trace from="U7.MISO" to="net.SPI_MISO" />
    <trace from="U1.GPIO10" to="net.SD_CS" />
    <trace from="U1.GPIO6" to="net.SPI_SCK" />
    <trace from="U1.GPIO7" to="net.SPI_MOSI" />
    <trace from="U1.GPIO2" to="net.SPI_MISO" />

    {/* ---------------- I2C: BME280 (0x76) + BH1750 (ADDR low = 0x23) ---------------- */}
    <resistor
      name="R1"
      resistance="4.7k"
      footprint="0603"
      schX={10}
      schY={3}
      schOrientation="vertical"
    />
    <resistor
      name="R2"
      resistance="4.7k"
      footprint="0603"
      schX={11.5}
      schY={3}
      schOrientation="vertical"
    />
    <trace from="R1.pin1" to="net.V3V3" />
    <trace from="R2.pin1" to="net.V3V3" />
    <trace from="R1.pin2" to="net.I2C_SDA" />
    <trace from="R2.pin2" to="net.I2C_SCL" />
    <trace from="U1.GPIO0" to="net.I2C_SDA" />
    <trace from="U1.GPIO1" to="net.I2C_SCL" />

    <chip
      name="U3"
      manufacturerPartNumber="BME280"
      footprint="pinrow4"
      pinLabels={{ pin1: "VCC", pin2: "GND", pin3: "SCL", pin4: "SDA" }}
      schPinArrangement={{
        topSide: { pins: ["VCC"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: { pins: ["SCL", "SDA"], direction: "top-to-bottom" },
      }}
      schX={14}
      schY={3}
    />
    <trace from="U3.VCC" to="net.V3V3" />
    <trace from="U3.GND" to="net.GND" />
    <trace from="U3.SCL" to="net.I2C_SCL" />
    <trace from="U3.SDA" to="net.I2C_SDA" />

    <chip
      name="U4"
      manufacturerPartNumber="BH1750"
      footprint="pinrow5"
      pinLabels={{
        pin1: "VCC",
        pin2: "GND",
        pin3: "SCL",
        pin4: "SDA",
        pin5: "ADDR",
      }}
      schPinArrangement={{
        topSide: { pins: ["VCC"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: { pins: ["SCL", "SDA", "ADDR"], direction: "top-to-bottom" },
      }}
      schX={14}
      schY={-1}
    />
    <trace from="U4.VCC" to="net.V3V3" />
    <trace from="U4.GND" to="net.GND" />
    <trace from="U4.SCL" to="net.I2C_SCL" />
    <trace from="U4.SDA" to="net.I2C_SDA" />
    <trace from="U4.ADDR" to="net.GND" />

    {/* ---------------- 1-Wire: DS18B20 probe ---------------- */}
    <resistor
      name="R3"
      resistance="4.7k"
      footprint="0603"
      schX={11}
      schY={-5}
      schOrientation="vertical"
    />
    <chip
      name="U5"
      manufacturerPartNumber="DS18B20"
      footprint="pinrow3"
      pinLabels={{ pin1: "GND", pin2: "DQ", pin3: "VDD" }}
      schPinArrangement={{
        topSide: { pins: ["VDD"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: { pins: ["DQ"], direction: "top-to-bottom" },
      }}
      schX={14}
      schY={-5}
    />
    <trace from="R3.pin1" to="net.V3V3" />
    <trace from="R3.pin2" to="net.ONEWIRE" />
    <trace from="U5.DQ" to="net.ONEWIRE" />
    <trace from="U5.VDD" to="net.V3V3" />
    <trace from="U5.GND" to="net.GND" />
    <trace from="U1.GPIO3" to="net.ONEWIRE" />

    {/* ---------------- PMS5003 + 2N7000 low-side power switch ---------------- */}
    <chip
      name="U6"
      manufacturerPartNumber="PMS5003"
      footprint="pinrow8"
      pinLabels={pmsPins}
      schPinArrangement={{
        topSide: { pins: ["VCC"], direction: "left-to-right" },
        bottomSide: { pins: ["GND"], direction: "left-to-right" },
        leftSide: { pins: ["RXD", "TXD", "SET", "RESET"], direction: "top-to-bottom" },
        rightSide: { pins: ["NC7", "NC8"], direction: "top-to-bottom" },
      }}
      pinAttributes={{ VCC: { requiresPower: true } }}
      schX={7}
      schY={0}
    />
    <mosfet
      name="Q1"
      manufacturerPartNumber="2N7000"
      channelType="n"
      mosfetMode="enhancement"
      footprint="to92"
      schX={7}
      schY={-4}
    />
    <resistor
      name="R4"
      resistance="100k"
      footprint="0603"
      schX={5}
      schY={-4}
      schOrientation="vertical"
    />
    <trace from="U6.VCC" to="net.V5" />
    <trace from="U6.GND" to="net.PMS_GND" />
    <trace from="U6.RXD" to="net.PMS_RX" />
    <trace from="U6.TXD" to="net.PMS_TX" />
    <trace from="U1.GPIO20" to="net.PMS_RX" />
    <trace from="U1.GPIO21" to="net.PMS_TX" />
    {/* GPIO23 -> gate; R4 keeps the PMS off while the GPIO floats (boot / deep sleep) */}
    <trace from="U1.GPIO23" to="net.PMS_EN" />
    <trace from="Q1.gate" to="net.PMS_EN" />
    <trace from="R4.pin1" to="net.PMS_EN" />
    <trace from="R4.pin2" to="net.GND" />
    <trace from="Q1.drain" to="net.PMS_GND" />
    <trace from="Q1.source" to="net.GND" />
    </schematicsheet>
  </board>
)
