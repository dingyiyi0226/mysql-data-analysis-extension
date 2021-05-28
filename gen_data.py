import sys

import numpy as np


N = 500
decimal = 3
x = np.linspace(0, 5, N)

noise = np.random.rand(N)
linear = x * 2 + noise
square = np.power(x, 2) + noise

linear = np.around(linear, decimal)
square = np.around(square, decimal)


if sys.platform.startswith('darwin'):
    """ plot the signal on local machine """

    import matplotlib.pyplot as plt

    plt.plot(x, linear, label='linear')
    plt.plot(x, square, label='square')
    plt.legend()
    plt.show()

elif sys.platform.startswith('linux'):
    """ insert to database on remote machine """

    import mysql.connector

    cnx = mysql.connector.connect(host='127.0.0.1', user='root', password='mysql', database='stats')
    cursor = cnx.cursor()

    if len(sys.argv) > 1 and sys.argv[1] == 'drop':
        print('Drop table')

        cursor.execute('DROP TABLE data')
        cursor.close()
        cnx.close()
        sys.exit()

    table_str = (
        "CREATE TABLE `data` ("
        "  `linear` DOUBLE,"
        "  `square` DOUBLE"
        ")"
    )

    cursor.execute(table_str)

    inserted_str = 'INSERT INTO data VALUES (%s, %s)'

    for items in zip(linear, square):
        cursor.execute(inserted_str, items)
    cnx.commit()

    cursor.close()
    cnx.close()
