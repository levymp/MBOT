import matplotlib.pyplot as plt
import sys
import streamlit as st

sys.path.append('utils')
import api_utils as utils
import app_utils


# Set Page Config and Title
st.beta_set_page_config(page_title='MBOT', page_icon="🚀", layout='centered', initial_sidebar_state='expanded')
st.title('MBOT Database & Analysis')

# get production and backup database values


# get all columns from data from
columns = ['BOT NAME', 'PICKLE NAME', 'PICKLE PATH', 'LOG NAME', 'LOG PATH', 'DATE', 'DESCRIPTION']


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # SIDEBAR --> DECIDE DATABASE # # # # # # # # # # # # # # # # # # # # # # # 

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


st.sidebar.write('## DATABASE')

# Decide Database
switch = st.sidebar.radio('',
                        options=('PRODUCTION', 'BACKUP'),
                        index=0,
                        key='x1')

st.sidebar.write('### COLUMNS TO DISPLAY')

# Decide Columns to display
display_columns = st.sidebar.multiselect('',
                                    options=columns,
                                    default=['BOT NAME', 'DATE', 'DESCRIPTION'],
                                    key='me4123')


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # MAIN PAGE --> DATABASE # # # # # # # # # # # # # # # # # # # # # # # # # # 

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

'''
## **MBOT DATABASE**
This is an easy way to visualize the data that is being posted from the [MBOT API](https://api.mplevy.com/api/). 
This API parses log files sent after each of our teams robots finishes driving. The production database stores all of our 
data that is important and we keep it *relatively* organized. Our backup database shows ***everything*** that has been sent to 
the production database (including deleted runs). 
'''

# switch on database type
if switch == 'PRODUCTION':
    '### ***PRODUCTION DATABASE***'
    df, botnames = app_utils.get_df('prod')
else:
    '### ***BACKUP DATABASE***'
    df, botnames = app_utils.get_df('backup')
    st.sidebar.error('*THIS IS ONLY TO VIEW BACKUP TABLE YOU* ***CANNOT*** *ANALYZE BACKUP DATA!*')

# get desired date range
st.sidebar.write('### DATE RANGE')
dates = st.sidebar.date_input('', value=[df.loc[0]['DATETIME'], df.loc[len(df)-1]['DATETIME']])

# get desired bot name
st.sidebar.write('### CHOOSE A ROBOT NAME')
botname = st.sidebar.selectbox('', options=botnames, index=0)

df = app_utils.filter_df(df, dates, botname)


# write df (prod or backup)
st.dataframe(df.assign(hack=df['Runs']).set_index('hack')[display_columns])

if not len(df):
    st.error('***NO RESULTS SET A NEW DATE RANGE!***')
    st.stop()
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # SIDEBAR --> PICK RUN # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# get runId of interest
st.sidebar.write('### RUN TO ANALYZE')
try:
    runId = st.sidebar.selectbox('',
                            list(df['Runs']),
                            index=0,
                            key='xy2')
except Exception as e:
    runId = 0

# reformat runId
runId = runId.replace('RUN ', '')
runId = int(runId)


# get df to analyze from runId
df_run = utils.get_df(runId)


'## ***SELECTED RUN TO ANALYZE***'
'#### ROBOT NAME'
# try:
st.code(df.loc[runId]['BOT NAME'])
# except Exception:
#     st.error('CHOOSE A VALID RUN ID')
#     st.stop()


'#### DESCRIPTION'
st.code(df.loc[runId]['DESCRIPTION'])

'#### DATE'
date = app_utils.file_to_time(df, runId, dateobj=False)
st.code(date)


# get lookup table
df_keys = app_utils.get_lookup(df_run.copy())


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # MAIN PAGE --> SHOW KEYS FOR FILE # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

'''### ***LCM CHANNELS OF SELECTED RUN***'''

# Show keys w/blank lines
st.dataframe(df_keys.replace({None:''}).assign(hack='').set_index('hack'))



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # SIDEBAR --> PLOTTING # # # # # # # # # # # # # # # # # # # # # # # # # # 

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

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

# random keys for selections
key_list = ['12', '123', '1234', '1234']

# Get selection from user
number_channels = len(channel_list)
yaxes = []
for key, channel, selection in zip(key_list, channel_list, selections):
    # write channel name to sidebar
    st.sidebar.write(f'## {selection}'.replace('_', ' '))
    # write y-axis to 
    yaxes.append(st.sidebar.selectbox('', options=channel, key=key))




# selection -> overall channel
# flag -> sub channel

# colors
palette = plt.get_cmap('Set1')

# amount of selections
rows = len(selections)

# stop if user hasn't selected channel
if rows:
    fig, axes = plt.subplots(rows, 1)
else:
    st.warning('## ***SELECT A CHANNEL TO VIEW ON THE SIDEBAR!***')
    st.stop()

# set font size based on amount of subplots
if rows == 1:
    font = 12
elif rows == 2:
    font = 9
elif rows == 3:
    font = 7
else:
    font = 4


'### **PLOT OF RUN FOR ALL DATA SELECTED**'
'#### LEGEND'

# keys for select box (wont need all of these)
key_list = ['123lksdjf', '019283091', '10923091283']

# i -> index for plots, channel -> user selected channel, yaxis -> value of interest from channel
for yaxis, channel, i in zip(yaxes, selections, range(rows)):
    # label
    label = '{0} : {1}'.format(channel.replace('_', ' '), yaxis.replace('_', ' ').upper())

    # display what is being plotted
    st.code(f'TIMESTAMP VS {label}', language='c++')

    # time (shifted from zero)
    time = app_utils.shift(df_run[channel]['timestamp'], time=True)

    # # y value (shifted) and if it's a list get the user to decide what axis to show
    try:
        y = app_utils.shift(df_run[channel][yaxis], time=False)
    except Exception:
        indx = st.sidebar.selectbox(label + ' CHOOSE DOF!',
                                    options=range(len(df_run[channel][yaxis][0])), 
                                    index=0,
                                    key=key_list[i])
        y = app_utils.shift([x[indx] for x in df_run['MBOT_IMU']['gyro']], time=False)

    # if > 1 selection use list otherwise use a single axis
    try:
        axes[i].set_xlabel('TIME (s)')
        axes[i].set_ylabel(label, color=palette(i), fontsize=font)
        axes[i].plot(time, y, color=palette(i))
    except Exception:
        axes.set_xlabel('TIME (s)')
        axes.set_ylabel(label, color=palette(i), fontsize=font)
        axes.plot(time, y, color=palette(i))

# plot subplot
'#### PLOT'
st.pyplot(fig)
