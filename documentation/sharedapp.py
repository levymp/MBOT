import streamlit as st
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import analysis_utils as utils
import app_utils
from datetime import datetime
from random import random
from part1 import part1
import plotly.express as px


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

st.warning('PART 1-3 will be documented here... ***updates coming***')

# switch on database type
if switch == 'PRODUCTION':
    '### ***Production***'
    st.dataframe(df_prod.assign(hack=indexes[0]).set_index('hack')[display_columns])
else:
    '### ***Backup***'
    st.sidebar.error('*THIS IS ONLY TO VIEW BACKUP TABLE YOU* ***CANNOT*** *ANALYZE BACKUP DATA!*')
    st.dataframe(df_backup.assign(hack=indexes[1]).set_index('hack')[display_columns])



# # # # # # ## # SIDE BAR
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
df_keys = app_utils.get_lookup(df_run.copy())




# # # # ## # # MAIN PAGE
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
                                    key='meow1')


# get parsed down list of options
channel_list = []
for selection in selections:
    channel_list.append(app_utils.get_dropdown_list(df_keys[selection]))

key_list = ['12', '123', '1234', '1234']

number_channels = len(channel_list)
yaxes = []
for key, channel, selection in zip(key_list, channel_list, selections):
    # write channel name to sidebar
    st.sidebar.write(f'## {selection}'.replace('_', ' '))
    # write y-axis to 
    yaxes.append(st.sidebar.selectbox('', options=channel, key=key))





'### **PLOT OF RUN FOR ALL DATA SELECTED**'
'#### LEGEND'


### GET THE POT
# selection -> overall channel
# flag -> sub channel

# plt.style.use('seaborn-darkgrid')

palette = plt.get_cmap('Set1')
count = 1
rows = len(selections)
fig, axes = plt.subplots(rows, 1)


for yaxis, channel, axis in zip(yaxes, selections, axes):
    # display what is being plotted
    st.code('timestamp vs {0}:{1}'.format(channel.replace('_', ' '), yaxis.replace('_', ' ').upper()))
    
    # label
    label = '{0}:{1}'.format(channel.replace('_', ' '), yaxis.replace('_', ' ').upper())
    
    # time (shifted from zero)
    time = app_utils.shift(df_run[channel]['timestamp'], time=True)

    # # y value (shifted)
    y = app_utils.shift(df_run[channel][yaxis], time=False)
    
    # plt dataframe
    # plt.plot(time, y, color=palette(count), linewidth=1.5, alpha=0.9, label=f'{channel}-{yaxis}')

    axis.set_xlabel('TIME (s)')
    axis.set_ylabel(label, color=palette(count))
    axis.plot(time, y, color=palette(count))
    # axes[i].tick_params(axis='y', labelcolor=palette(count))

    # change pallette count
    count += 1

st.pyplot(fig)

# Create some mock data




# axes.append(ax1.twinx())   # instantiate a second axes that shares the same x-axis


# ax2.set_ylabel('sin', color=color)  # we already handled the x-label with ax1
# ax2.plot(t, data2, color=color)
# ax2.tick_params(axis='y', labelcolor=color)

# fig.tight_layout()  # otherwise the right y-label is slightly clipped









# write to figure
# fig = px.line(all_df[0], x='time', y=yaxes, title="time vs. " + yaxis)

# display figure
# st.plotly_chart(fig)
