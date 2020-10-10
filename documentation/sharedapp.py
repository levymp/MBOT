import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import analysis_utils as utils
import app_utils
from datetime import datetime

# Set Page Config and Title
st.beta_set_page_config(page_title='MBOT', page_icon="🚀", layout='centered', initial_sidebar_state='collapsed')
st.title('MBOT Database, Analysis, and Report')

# if on linux box it will just grab file... if running locally will use api
df_prod, df_backup = app_utils.get_tables()

'## MBOT DATABASE'

# set desired columns
columns = ['BOT NAME', 'PICKLE NAME', 'LOG NAME', 'DESCRIPTION']
switch = st.sidebar.radio(
    'What Database would you like to view?',
    ('PRODUCTION', 'BACKUP'),
    index=0,
    key='1f2'
)

# switch on database type
if switch == 'PRODUCTION':
    '### ***Production***'
    st.write(df_prod[columns])
else:
    '### ***Backup***'
    st.write(df_backup[columns])



# get runId of interest
st.sidebar.warning('You can only view ***production*** database')
runId = st.sidebar.selectbox('SELECT FILE TO ANALYZE',
            range(len(df_prod)),
            index=0,
            key='dfsg')

# get df to analyze
df_run = utils.get_df(runId)


'##### Robot Name'
st.code(df_prod.loc[runId]['BOT NAME'])

'##### Description'
st.code(df_prod.loc[runId]['DESCRIPTION'])

'##### Date'
date = app_utils.file_to_time(df_prod, runId, dateobj=False)
st.code(date)


# get lookup table
df_keys = app_utils.get_lookup(df_run)

'''### ***LCM Channels of Selected File***'''
df_keys


# plotting
'''
### Pick your values 
'''
cols = df_keys.keys()

selections = st.sidebar.multiselect('PICK CHANNELS TO DISPLAY', cols)

# vals = list_keys(df['MBOT_IMU'].keys())


def get_dropdown_list(series):
    '''Given pandas series return list w/out None and timestamp/utime'''
    series = series.dropna()
    series = series.tolist()
    series.remove('timestamp')
    series.remove('utime')
    return series

first = get_dropdown_list(df_keys[selections[0]])
second
if len(options) == 0: 
    pass
elif len(options) == 1:
    key1 = st.sidebar.selectbox(selections[0], series, key='x1')
elif len(options) == 2:
    key1 = st.sidebar.selectbox(options[0], list_keys(df[options[0]].keys()), key='x1')
    key2 = st.sidebar.selectbox(keys[1], list_keys(df[options[1]].keys()), key='x2')
elif len(options) == 3:
    key1 = st.sidebar.selectbox(options[0], list_keys(df[options[0]].keys()), key='x1')
    key2 = st.sidebar.selectbox(keys[1], list_keys(df[options[1]].keys()), key='x2')
    key3 = st.sidebar.selectbox(keys[2], list_keys(df[options[2]].keys()), key='x3')
else:
    st.warning('uh oh!! need to add more sidebar options')

# # get the plot
# for key in df['MBOT_MOTOR_COMMAND']:
#     if(key != 'timestamp' and key != 'utime'):
#         x = plt.plot(df['MBOT_MOTOR_COMMAND']['timestamp'], df['MBOT_MOTOR_COMMAND'][key], label=key)
# plt.legend()
# st.pyplot()
