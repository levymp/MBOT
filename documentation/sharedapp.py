import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import analysis_utils as utils
import app_utils
from datetime import datetime
from random import random


# Set Page Config and Title
st.beta_set_page_config(page_title='MBOT', page_icon="🚀", layout='centered', initial_sidebar_state='expanded')
st.title('MBOT Database, Analysis, and Report')
# st.set_option('deprecation.showPyplotGlobalUse', False)

# if on linux box it will just grab file... if running locally will use api
df_prod, df_backup = app_utils.get_tables()

'## MBOT DATABASE'

# set desired columns
columns = ['BOT NAME', 'PICKLE NAME', 'LOG NAME', 'DESCRIPTION']


st.sidebar.warning('You can only view runs in the ***production*** database!')

switch = st.sidebar.radio('Database',
                        options=('PRODUCTION', 'BACKUP'),
                        index=0,
                        key='x1')

# switch on database type
if switch == 'PRODUCTION':
    '### ***Production***'
    df_prod[columns]
else:
    '### ***Backup***'
    df_backup[columns]

# get runId of interest
st.sidebar.write('### SELECT RUN TO ANALYZE')
runId = st.sidebar.selectbox('RUN',
                            range(len(df_prod)),
                            index=0,
                            key='xy2')

# get df to analyze
df_run = utils.get_df(runId)

'## Selected Run to Analyze'
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
st.sidebar.write('''### SELECT CHANNELS''')
# Get all columns and remove time sync
cols = df_keys.keys()
cols = list(cols)
cols.remove('MBOT_TIMESYNC')

# multi select the channel to view
selections = st.sidebar.multiselect('CHANNELS',
                                    options=cols,
                                    default=['SLAM_PARTICLES', 'LIDAR'],
                                    key='meow1')


# get parsed down list of options
channel_list = []
for selection in selections:
    channel_list.append(app_utils.get_dropdown_list(df_keys[selection]))

key_list = ['12', '123', '1234', '1234']

number_channels = len(channel_list)
yaxes = []
for key, channel, selection in zip(key_list, channel_list, selections):
    yaxes.append(st.sidebar.selectbox(selection, options=channel, key=key))

### get plot
# selection -> overall channel
# flag -> sub channel
for yaxis, channel in zip(yaxes, selections):
    st.write(yaxis)
    plt.plot([df_run[channel]['utime'], df_run[channel][yaxis]])
    
# fig = plt.show()
st.pyplot()



# for key in df_run['MBOT_MOTOR_COMMAND']:
#     if(key != 'timestamp' and key != 'utime'):
#         x = plt.plot(df['MBOT_MOTOR_COMMAND']['timestamp'], df_run['MBOT_MOTOR_COMMAND'][key], label=key)
# plt.legend()
# st.pyplot()
