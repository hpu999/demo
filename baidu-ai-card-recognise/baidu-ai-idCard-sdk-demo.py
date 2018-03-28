# -*- coding: utf-8 -*-
from aip import AipOcr
import json

APP_ID = '10999413'
API_KEY = 'oNzQDojuKDSeC8xKFApL5E8x'
SECRET_KEY = 'fXhtDT47iQVO6kBneTR3GXvYtL4wkpjv'

client = AipOcr(APP_ID, API_KEY, SECRET_KEY)

""" 读取图片 """
def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()

image = get_file_content('id.jpg')
# front 读取身份证的正面，含照片的一面
# back  读取身份证的背面，含国徽的一面
idCardSide = "front"

""" 调用身份证识别 """
client.idcard(image, idCardSide);

# 将json数据显示为汉字(dumps方法)
print (json.dumps (client.idcard(image, idCardSide), encoding='UTF-8', ensure_ascii=False));


