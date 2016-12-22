from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker


#engine = create_engine('mysql://precibeo@locahost/targetList')
engine = create_engine('sqlite:///elib_db.sqlite', echo=True)

Session = sessionmaker(bind=engine)
session = Session()
