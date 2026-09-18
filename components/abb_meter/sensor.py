import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import modbus, sensor
from esphome.const import (
    CONF_ADDRESS,
    CONF_ID,
    CONF_MODBUS_ID,
    CONF_UPDATE_INTERVAL,
)

DEPENDENCIES = ["modbus"]

abb_meter_ns = cg.esphome_ns.namespace("abb_meter")
ABBMeterSensor = abb_meter_ns.class_(
    "ABBMeterSensor", cg.PollingComponent, modbus.ModbusDevice
)

CONF_REGISTER_ADDRESS = "register_address"
CONF_VALUE_TYPE = "value_type"
CONF_SCALE = "scale"
CONF_SWAP_WORDS = "swap_words"
CONF_REGISTER_COUNT = "register_count"

VALUE_TYPES = {
    "U_WORD": 1,
    "S_WORD": 1,
    "U_DWORD": 2,
    "S_DWORD": 2,
    "U_QWORD": 4,
    "S_QWORD": 4,
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(ABBMeterSensor)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(ABBMeterSensor),
            cv.GenerateID(CONF_MODBUS_ID): cv.use_id(modbus.ModbusComponent),
            cv.Required(CONF_ADDRESS): cv.int_range(min=1, max=247),
            cv.Required(CONF_REGISTER_ADDRESS): cv.hex_uint16_t,
            cv.Required(CONF_VALUE_TYPE): cv.enum(VALUE_TYPES, upper=True),
            cv.Optional(CONF_SCALE, default=1.0): cv.float_,
            cv.Optional(CONF_SWAP_WORDS, default=False): cv.boolean,
            cv.Optional(CONF_REGISTER_COUNT): cv.int_range(min=1, max=4),
            cv.Optional(CONF_UPDATE_INTERVAL, default="10s"): cv.update_interval,
        }
    )
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await modbus.register_modbus_device(var, config)
    words = config.get(CONF_REGISTER_COUNT, VALUE_TYPES[config[CONF_VALUE_TYPE]])
    cg.add(var.set_register_address(config[CONF_REGISTER_ADDRESS]))
    cg.add(var.set_value_type(config[CONF_VALUE_TYPE]))
    cg.add(var.set_scale(config[CONF_SCALE]))
    cg.add(var.set_swap_words(config[CONF_SWAP_WORDS]))
    cg.add(var.set_register_count(words))
