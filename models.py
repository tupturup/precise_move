from sqlalchemy import Column, DateTime, String, Integer, ForeignKey, func, Float, Table
from sqlalchemy.orm import relationship, backref
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

#target_path = Table('target_path', Base.metadata,
#                    Column('target_id', Integer, ForeignKey('tbl_target.id')),
#                    Column('path_id', Integer, ForeignKey('tbl_path.id'))
#                    )

class Target(Base):
    __tablename__ = 'tbl_target'

    id = Column(Integer, primary_key=True)
    name = Column(String, unique=True, nullable=False)
    value_x = Column(Float)
    value_y = Column(Float)
    value_z = Column(Float)
#    belongspaths = relationship('Path', secondary=target_path)

    def __init__(self, name, value_x, value_y, value_z):
        self.name = name
        self.value_x = value_x
        self.value_y = value_y
        self.value_z = value_z

    def __unicode__(self):
        return self.name

#DO WE NEED PATH???
#class Path(Base):
#    __tablename__= 'tbl_path'
#
#    id = Column(Integer, primary_key=True)
#    name = Column(String, unique=True, nullable=False)
#    belongtarget = relationship('Target', secondary=target_path)
#
#    def __init__(self, name):
#        self.name = name
