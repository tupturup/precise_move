from wtforms import Form, StringField, validators
from  sql import session
from models import Target

def get_target():
    return session.query(Target).all()

class TargetForm(Form):
    tgt_name = StringField('Name', [validators.input_required()])
    value_x = StringField('X', [validators.input_required()])
    value_y = StringField('Y', [validators.input_required()])
    value_z = StringField('Z', [validators.input_required()])
