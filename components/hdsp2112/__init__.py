import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.const import CONF_ID

DEPENDENCIES = ["esp32"]

hdsp2112_ns = cg.esphome_ns.namespace("hdsp2112")

HDSP2112Component = hdsp2112_ns.class_(
    "HDSP2112Component",
    cg.Component,
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HDSP2112Component),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
