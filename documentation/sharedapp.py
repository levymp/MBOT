import sys
import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
# from utils.utils import get_lookup

st.title('MBOT ANALYSIS TOOL')

'## Lookup Table'
mbot_table_path = Path('/home/michaellevy/data/mbot/mbot_table.pkl')
mbot_table = pd.read_pickle(mbot_table_path)
columns = ['BOT NAME', 'PICKLE NAME', 'LOG NAME']
st.table(mbot_table[columns])



def get_df(file_path):
    df = pd.read_pickle(file_path)
    return df

# display all keys
def get_lookup(df):
    # start
    lookup = {}
    values = []
    
    # go through all keys
    for key1 in df.keys():
        for key2 in df[key1].keys():
            # append all keys to a list 
            values.append(key2)
        # add list of keys to lcm type dictionary
        lookup[key1] = values.copy()
        # clear values
        values.clear()
    # write to pandas df
    df_lookup = pd.DataFrame.from_dict(lookup, orient='index')
    df_lookup = df_lookup.transpose()
    # hello
    return df_lookup


# # get all keys
# df_lookup = get_lookup(df)

# '''
# ## LCM TYPES USED
# '''
# # display all keys
# df_lookup

# # plotting
# '''
# ### Pick your values 
# '''
# keys = list(df.keys())

# options = st.sidebar.multiselect('PICK CHANNELS TO DISPLAY', keys)

# def list_keys(keys):
#     stor = []
#     for key in keys:
#         if str(key) != 'timestamp' or str(key) != 'utime':
#             stor.append(key)
#     return tuple(stor)

# vals = list_keys(df['MBOT_IMU'].keys())


# # 
# if len(options) == 0: 
#     pass
# elif len(options) == 1:
#     key1 = st.sidebar.selectbox(options[0], list_keys(df[options[0]].keys()), key='x1')
# elif len(options) == 2:
#     key1 = st.sidebar.selectbox(options[0], list_keys(df[options[0]].keys()), key='x1')
#     key2 = st.sidebar.selectbox(keys[1], list_keys(df[options[1]].keys()), key='x2')
# elif len(options) == 3:
#     key1 = st.sidebar.selectbox(options[0], list_keys(df[options[0]].keys()), key='x1')
#     key2 = st.sidebar.selectbox(keys[1], list_keys(df[options[1]].keys()), key='x2')
#     key3 = st.sidebar.selectbox(keys[2], list_keys(df[options[2]].keys()), key='x3')
# else:
#     st.warning('uh oh!! need to add more sidebar options')

# # get the plot
# for key in df['MBOT_MOTOR_COMMAND']:
#     if(key != 'timestamp' and key != 'utime'):
#         x = plt.plot(df['MBOT_MOTOR_COMMAND']['timestamp'], df['MBOT_MOTOR_COMMAND'][key], label=key)
# plt.legend()
# st.pyplot()
