import streamlit as st
import pandas as pd

st.beta_set_page_config(page_title='MBOT', page_icon="🚀", layout='centered', initial_sidebar_state='collapsed')

st.title('MBot Setup')

st.write('''
***Please ignore the cheesy music in my videos 😊.***

## **Assembly**   
*Timelapse of robot assembly.*
''')

# assembly timelapse
st.video('https://www.youtube.com/watch?v=HLtRFjogLS4')

st.write('''
## **Simple Driving**   
*MBOT driving around using the teleop_simple program.*
''')

# teleop video
st.video('https://www.youtube.com/watch?v=lfkwy5WPypM')


st.write('''
## **MBOT Driven by timer:**   
*Simple program written to control robot based purely on time. This was using my best 
guess as to the time it took the robot to turn or travel 1 meter.*'''
)

# Video of MBOT driving around in naive square
# st.video()

# 
st.write('''
## **MBOT Driven by encoder readings:**   
*MBOT driving around on three different surfaces using the encoders to track distance. See on screen display to show encoder readings and distance traveled.*'''
)
st.video('https://www.youtube.com/watch?v=XQGEAMSEiGs')


st.write('''### **Encoder Data**  
*This is data from each encoder in order to measure the average rotation of one wheel.*  
*Calculating the average rotation helped me build the logic around a 90&deg turn or covering one 1 meter.*''')

df = pd.read_excel('/home/michaellevy/projects/MBOT/documentation/encoder_values.xlsx')
st.table(df)


st.write('''
## **Observations**
### Surface Performance:  
The MBOT performed well on hardwood floors and polished tiles. 

### Turning:



''')
