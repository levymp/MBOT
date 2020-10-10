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
st.set_option('deprecation.showPyplotGlobalUse', False)

# get production and backup database values
df_prod, df_backup, indexes = app_utils.get_tables()

# get all columns from data from
columns = ['BOT NAME', 'PICKLE NAME', 'PICKLE PATH', 'LOG NAME', 'LOG PATH', 'DATE', 'DESCRIPTION']

# # # # # # # # SIDEBAR
st.sidebar.write('### DATA TO DISPLAY')
display_columns = st.sidebar.multiselect('',
                                    options=columns,
                                    default=['BOT NAME', 'DATE', 'DESCRIPTION'],
                                    key='me4123')

st.sidebar.write('### DATABASE')
switch = st.sidebar.radio('',
                        options=('PRODUCTION', 'BACKUP'),
                        index=0,
                        key='x1')

# # # # # # # # MAIN PAGE
'## **MBOT DATABASE**'
'''
This is an easy way to visualize the data that is being posted from the [MBOT API](https://api.mplevy.com/api/). 
This API parses log files sent after each of our teams robots finishes driving. The production database stores all of our 
data that is important and we keep it *relatively* organized. Our backup database shows ***everything*** that has been sent to 
the production database (including deleted runs). 


'''




# switch on database type
if switch == 'PRODUCTION':
    '### ***Production***'
    st.dataframe(df_prod.assign(hack=indexes[0]).set_index('hack')[display_columns])
else:
    '### ***Backup***'
    st.sidebar.warning('*THIS IS ONLY TO VIEW BACKUP TABLE YOU* ***CANNOT*** *ANALYZE BACKUP DATA!*')
    st.dataframe(df_backup.assign(hack=indexes[1]).set_index('hack')[display_columns])

# get runId of interest
st.sidebar.write('### RUN TO ANALYZE')
runId = st.sidebar.selectbox('',
                            range(len(df_prod)),
                            index=0,
                            key='xy2')

# get df to analyze
df_run = utils.get_df(runId)

'## ***Selected Run to Analyze***'
'#### Robot Name'
st.code(df_prod.loc[runId]['BOT NAME'])

'#### Description'
st.code(df_prod.loc[runId]['DESCRIPTION'])

'#### Date'
date = app_utils.file_to_time(df_prod, runId, dateobj=False)
st.code(date)


# get lookup table
df_keys = app_utils.get_lookup(df_run)

'''### ***LCM Channels of Selected File***'''

# Show keys w/blank lines
st.dataframe(df_keys.replace({None:''}).assign(hack='').set_index('hack'))


# plotting
st.sidebar.write('''### LCM CHANNELS''')
# Get all columns and remove time sync
cols = df_keys.keys()
cols = list(cols)
cols.remove('MBOT_TIMESYNC')

# multi select the channel to view
selections = st.sidebar.multiselect('',
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
    st.sidebar.write(f'## {selection}'.replace('_', ' '))
    yaxes.append(st.sidebar.selectbox('', options=channel, key=key))

'### **PLOT OF RUN FOR ALL DATA SELECTED**'
'#### LEGEND'


### GET THE POT
# selection -> overall channel
# flag -> sub channel
for yaxis, channel in zip(yaxes, selections):
    st.code('utime vs {0}:{1}'.format(channel.replace('_', ' '), yaxis.replace('_', ' ').upper()))
    plt.plot([df_run[channel]['utime'], df_run[channel][yaxis]])
# plot data
st.pyplot()
