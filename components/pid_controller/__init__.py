import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL
from esphome.components import sensor, binary_sensor, number, select

pid_ns = cg.esphome_ns.namespace('pid_controller')
PIDController = pid_ns.class_('PIDController', cg.Component)

CONF_INPUT_SENSOR               = 'input_sensor'
CONF_SETPOINT_NUMBER           = 'setpoint_number'
CONF_KP_NUMBER                 = 'kp_number'
CONF_KI_NUMBER                 = 'ki_number'
CONF_KD_NUMBER                 = 'kd_number'
CONF_OUTPUT_SENSOR             = 'output_sensor'
CONF_STATE_SENSOR              = 'state_sensor'
CONF_MANUAL_OUTPUT_NUMBER      = 'manual_output_number'
CONF_SAMPLE_TIME_NUMBER        = 'sample_time_number'
CONF_OUTPUT_MIN_NUMBER         = 'output_min_number'
CONF_OUTPUT_MAX_NUMBER         = 'output_max_number'
CONF_PROPORTIONAL_ON_SELECT    = 'proportional_on_select'
CONF_CONTROLLER_DIRECTION_SELECT = 'controller_direction_select'
CONF_CONTROLLER_MODE_SELECT    = 'controller_mode_select'
CONF_SAMPLING_MODE_SELECT      = 'sampling_mode_select'
CONF_THRESHOLD                 = 'threshold'
CONF_THRESHOLD_NUMBER         = 'threshold_number'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PIDController),
    cv.Required(CONF_INPUT_SENSOR):    cv.use_id(sensor.Sensor),
    cv.Required(CONF_SETPOINT_NUMBER): cv.use_id(number.Number),
    cv.Required(CONF_KP_NUMBER):       cv.use_id(number.Number),
    cv.Required(CONF_KI_NUMBER):       cv.use_id(number.Number),
    cv.Required(CONF_KD_NUMBER):       cv.use_id(number.Number),
    cv.Required(CONF_OUTPUT_SENSOR):   cv.use_id(sensor.Sensor),
    cv.Required(CONF_STATE_SENSOR):    cv.use_id(binary_sensor.BinarySensor),
    cv.Optional(CONF_MANUAL_OUTPUT_NUMBER): cv.use_id(number.Number),
    cv.Optional(CONF_SAMPLE_TIME_NUMBER):   cv.use_id(number.Number),
    cv.Optional(CONF_OUTPUT_MIN_NUMBER):    cv.use_id(number.Number),
    cv.Optional(CONF_OUTPUT_MAX_NUMBER):    cv.use_id(number.Number),
    cv.Optional(CONF_PROPORTIONAL_ON_SELECT):    cv.use_id(select.Select),
    cv.Optional(CONF_CONTROLLER_DIRECTION_SELECT): cv.use_id(select.Select),
    cv.Optional(CONF_CONTROLLER_MODE_SELECT):    cv.use_id(select.Select),
    cv.Optional(CONF_SAMPLING_MODE_SELECT):      cv.use_id(select.Select),
    cv.Optional(CONF_THRESHOLD, default=0.0): cv.float_,
    cv.Optional(CONF_THRESHOLD_NUMBER):      cv.use_id(number.Number),
})

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    input_sensor = yield cg.get_variable(config[CONF_INPUT_SENSOR])
    setpoint_number = yield cg.get_variable(config[CONF_SETPOINT_NUMBER])
    kp_number = yield cg.get_variable(config[CONF_KP_NUMBER])
    ki_number = yield cg.get_variable(config[CONF_KI_NUMBER])
    kd_number = yield cg.get_variable(config[CONF_KD_NUMBER])
    output_sensor = yield cg.get_variable(config[CONF_OUTPUT_SENSOR])
    state_sensor = yield cg.get_variable(config[CONF_STATE_SENSOR])
    
    cg.add(var.set_input_sensor(input_sensor))
    cg.add(var.set_setpoint_number(setpoint_number))
    cg.add(var.set_kp_number(kp_number))
    cg.add(var.set_ki_number(ki_number))
    cg.add(var.set_kd_number(kd_number))
    cg.add(var.set_output_sensor(output_sensor))
    cg.add(var.set_state_sensor(state_sensor))
    cg.add(var.set_threshold(config[CONF_THRESHOLD]))
    
    # Optional sensors
    if CONF_MANUAL_OUTPUT_NUMBER in config:
        manual_output_number = yield cg.get_variable(config[CONF_MANUAL_OUTPUT_NUMBER])
        cg.add(var.set_manual_output_number(manual_output_number))
    
    if CONF_SAMPLE_TIME_NUMBER in config:
        sample_time_number = yield cg.get_variable(config[CONF_SAMPLE_TIME_NUMBER])
        cg.add(var.set_sample_time_number(sample_time_number))
    
    if CONF_OUTPUT_MIN_NUMBER in config:
        output_min_number = yield cg.get_variable(config[CONF_OUTPUT_MIN_NUMBER])
        cg.add(var.set_output_min_number(output_min_number))
    
    if CONF_OUTPUT_MAX_NUMBER in config:
        output_max_number = yield cg.get_variable(config[CONF_OUTPUT_MAX_NUMBER])
        cg.add(var.set_output_max_number(output_max_number))
    
    if CONF_PROPORTIONAL_ON_SELECT in config:
        proportional_on_select = yield cg.get_variable(config[CONF_PROPORTIONAL_ON_SELECT])
        cg.add(var.set_proportional_on_select(proportional_on_select))
    
    if CONF_CONTROLLER_DIRECTION_SELECT in config:
        controller_direction_select = yield cg.get_variable(config[CONF_CONTROLLER_DIRECTION_SELECT])
        cg.add(var.set_controller_direction_select(controller_direction_select))
    
    if CONF_CONTROLLER_MODE_SELECT in config:
        controller_mode_select = yield cg.get_variable(config[CONF_CONTROLLER_MODE_SELECT])
        cg.add(var.set_controller_mode_select(controller_mode_select))
    
    if CONF_SAMPLING_MODE_SELECT in config:
        sampling_mode_select = yield cg.get_variable(config[CONF_SAMPLING_MODE_SELECT])
        cg.add(var.set_sampling_mode_select(sampling_mode_select))
    
    if CONF_THRESHOLD_NUMBER in config:
        threshold_number = yield cg.get_variable(config[CONF_THRESHOLD_NUMBER])
        cg.add(var.set_threshold_number(threshold_number))