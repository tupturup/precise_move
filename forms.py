from wtforms import Form, StringField, validators, FloatField, ValidationError
from  sql import session
from models import Target

def get_target():
    return session.query(Target).all()

def type_num(form, field):
    if type(field.data) == str:
        raise ValidationError('You must enter a number')

class TargetForm(Form):
    tgt_name = StringField('Name', [validators.Required("Target name missing!"), validators.Length(min=3, max=10, message="Name must have between 3 and 10 characters.")])
    value_x = FloatField('X', [validators.Required("Axis value missing!"), type_num])
    value_y = FloatField('Y', [validators.Required("Axis value missing!"), type_num])
    value_z = FloatField('Z', [validators.Optional(), type_num])
