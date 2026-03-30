import numpy as np
from sklearn.linear_model import LinearRegression

x = np.random.rand(1_000_000, 1)
y = 3 * x.squeeze() + 2 + np.random.randn(1_000_000)
model = LinearRegression()
model.fit(x, y)
print(model.coef_)
print(model.intercept_)