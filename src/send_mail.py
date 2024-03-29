from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import smtplib
import json
import ssl

senderMail = 'your_email@gmail.com'
password = 'your_password'

smtp = smtplib.SMTP_SSL('smtp.gmail.com', 465, context=ssl._create_unverified_context())
smtp.login(senderMail, password)

jsonData = []
with open('whitelist.json') as whitelistFile:
    jsonData = json.load(whitelistFile)

checkerConfigFile = open('checker_config.json')
checkerConfig = json.load(checkerConfigFile)

for voter in jsonData['voters']:
    email = MIMEMultipart()
    email['From'] = senderMail
    email['Subject'] = 'Your configuration file for voteChain'
    email['To'] = voter["email"]
    email.attach(MIMEText("File checker_config.json is in attachments. Copy it to folder where your client program is executed.", 'plain'))
    attachment = MIMEText(json.dumps(checkerConfig))
    attachment.add_header('Content-Disposition', 'attachment', filename="checker_config.json")
    email.attach(attachment)
    smtp.sendmail(senderMail, voter["email"], email.as_string())

smtp.close()
whitelistFile.close()
