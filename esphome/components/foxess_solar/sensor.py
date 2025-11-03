import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ACTIVE_POWER,
    CONF_CURRENT,
    CONF_FREQUENCY,
    CONF_ID,
    CONF_PHASE_A,
    CONF_PHASE_B,
    CONF_PHASE_C,
    CONF_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_HERTZ,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_WATT,
)

CONF_FLOW_CONTROL_PIN = "flow_control_pin"
CONF_ENERGY_PRODUCTION_DAY = "energy_production_day"
CONF_TOTAL_ENERGY_PRODUCTION = "total_energy_production"
CONF_PV1 = "pv1"
CONF_PV2 = "pv2"
CONF_PV3 = "pv3"
CONF_PV4 = "pv4"

CONF_LOADS_POWER = "loads_power"
CONF_GRID_POWER = "grid_power"
CONF_GENERATION_POWER = "generation_power"

CONF_INVERTER_STATUS = "inverter_status"
CONF_INVERTER_TEMP = "inverter_temp"
CONF_BOOST_TEMP = "boost_temp"
CONF_AMBIENT_TEMP = "ambient_temp"

DEPENDENCIES = ["uart"]

foxess_solar_ns = cg.esphome_ns.namespace("foxess_solar")
FoxessSolar = foxess_solar_ns.class_("FoxessSolar", cg.PollingComponent, uart.UARTDevice)

PHASE_SENSORS = {
    CONF_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_CURRENT: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_FREQUENCY,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
}

PV_SENSORS = {
    CONF_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_CURRENT: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
}

PHASE_SCHEMA = cv.Schema({cv.Optional(sensor): schema for sensor, schema in PHASE_SENSORS.items()})
PV_SCHEMA = cv.Schema({cv.Optional(sensor): schema for sensor, schema in PV_SENSORS.items()})

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(FoxessSolar),
            cv.Optional(CONF_FLOW_CONTROL_PIN, default="GPIO4"): pins.gpio_output_pin_schema,
            cv.Optional(CONF_PHASE_A): PHASE_SCHEMA,
            cv.Optional(CONF_PHASE_B): PHASE_SCHEMA,
            cv.Optional(CONF_PHASE_C): PHASE_SCHEMA,
            cv.Optional(CONF_PV1): PV_SCHEMA,
            cv.Optional(CONF_PV2): PV_SCHEMA,
            cv.Optional(CONF_PV3): PV_SCHEMA,
            cv.Optional(CONF_PV4): PV_SCHEMA,
            cv.Optional(CONF_INVERTER_STATUS): sensor.sensor_schema(),
            cv.Optional(CONF_LOADS_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_GRID_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_GENERATION_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_INVERTER_TEMP): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_AMBIENT_TEMP): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BOOST_TEMP): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ENERGY_PRODUCTION_DAY): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOWATT_HOURS,
                accuracy_decimals=1,
                icon="mdi:solar-power",
                device_class=DEVICE_CLASS_ENERGY,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
            cv.Optional(CONF_TOTAL_ENERGY_PRODUCTION): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOWATT_HOURS,
                accuracy_decimals=1,
                icon="mdi:solar-power",
                device_class=DEVICE_CLASS_ENERGY,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # flow control pin
    flow_control_pin = await cg.gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
    cg.add(var.set_fc_pin(flow_control_pin))

    # simple top-level sensors
    for key in [
        CONF_INVERTER_STATUS,
        CONF_LOADS_POWER,
        CONF_GRID_POWER,
        CONF_GENERATION_POWER,
        CONF_INVERTER_TEMP,
        CONF_AMBIENT_TEMP,
        CONF_BOOST_TEMP,
        CONF_ENERGY_PRODUCTION_DAY,
        CONF_TOTAL_ENERGY_PRODUCTION,
    ]:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(sens))

    # phases (A, B, C)
    for i, phase_key in enumerate([CONF_PHASE_A, CONF_PHASE_B, CONF_PHASE_C]):
        if phase_key not in config:
            continue
        phase_conf = config[phase_key]
        for sensor_type in PHASE_SENSORS:
            if sensor_type in phase_conf:
                sens = await sensor.new_sensor(phase_conf[sensor_type])
                cg.add(getattr(var, f"set_phase_{sensor_type}_sensor")(i, sens))

    # PVs (1..4)
    for i, pv_key in enumerate([CONF_PV1, CONF_PV2, CONF_PV3, CONF_PV4]):
        if pv_key not in config:
            continue
        pv_conf = config[pv_key]
        for sensor_type in PV_SENSORS:
            if sensor_type in pv_conf:
                sens = await sensor.new_sensor(pv_conf[sensor_type])
                cg.add(getattr(var, f"set_pv_{sensor_type}_sensor")(i, sens))
