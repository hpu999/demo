# coding:utf-8
import urllib, urllib2, base64

access_token = '24.dff8d7980e8fce8a1fdb917cdf4e79cc.2592000.1524708973.282335-10999413'
url = 'https://aip.baidubce.com/rest/2.0/ocr/v1/bankcard?access_token=' + access_token
f = open(r'example.jpg', 'rb')  # 二进制方式打开图文件
img = base64.b64encode(f.read())
params = {"image": img}
params = urllib.urlencode(params)
request = urllib2.Request(url, params)
request.add_header('Content-Type', 'application/x-www-form-urlencoded')
response = urllib2.urlopen(request)
content = response.read()
if (content):
    print(content)

