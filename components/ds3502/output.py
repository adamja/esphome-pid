import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

CONF_INVERTED = "inverted"

from . import DS3502Component, ds3502_ns

DEPENDENCIES = ["ds3502"]

DS3502Output = ds3502_ns.class_("DS3502Output", output.FloatOutput, cg.Component)

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(DS3502Output),
        cv.GenerateID("ds3502_id"): cv.use_id(DS3502Component),
        cv.Optional(CONF_INVERTED, default=False): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await output.register_output(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config["ds3502_id"])
    cg.add(var.set_parent(parent))
    
    if config[CONF_INVERTED]:
        cg.add(var.set_inverted(True))
