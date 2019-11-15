

def SingleStr2Num(ch):
    """
    单个字符转成数字
    Example:
    F --> 15
    """
    result = 0

    if str(ch) == "0":
        result = 0
    elif str(ch) == "1":
        result = 1
    elif str(ch) == "2":
        result = 2
    elif str(ch) == "3":
        result = 3
    elif str(ch) == "4":
        result = 4
    elif str(ch) == "5":
        result = 5
    elif str(ch) == "6":
        result = 6
    elif str(ch) == "7":
        result = 7
    elif str(ch) == "8":
        result = 8
    elif str(ch) == "9":
        result = 9
    elif str(ch) == "A":
        result = 10
    elif str(ch) == "B":
        result = 11
    elif str(ch) == "C":
        result = 12
    elif str(ch) == "D":
        result = 13
    elif str(ch) == "E":
        result = 14
    elif str(ch) == "F":
        result = 15
    elif str(ch) == "a":
        result = 10
    elif str(ch) == "b":
        result = 11
    elif str(ch) == "c":
        result = 12
    elif str(ch) == "d":
        result = 13
    elif str(ch) == "e":
        result = 14
    elif str(ch) == "f":
        result = 15

    return result

def Str2Hex(raw_data):
    """
    字符串 转 HEX
    Example :
    11 22 33 to 0x112233
    """

    result = []

    # 先去除一下空格
    str_tmp = ''.join(raw_data.split())

    # 如果长度为奇数，则在最后一个数字前补0
    if len(str_tmp)%2 == 1:
        new_str = str_tmp[0:-1] + "0" + str_tmp[-1]
    else:
        new_str = str_tmp

    # 字符转数字
    for x in range(0, len(new_str), 2):
        num = SingleStr2Num(new_str[x])*16 + SingleStr2Num(new_str[x+1])
        result.append(num)

    return result