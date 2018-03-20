import numpy as np
from scipy.optimize import minimize
from scipy.io import loadmat
from numpy.linalg import det, inv
from math import sqrt, pi, exp
import scipy.io
import matplotlib.pyplot as plt
import pickle
import sys

def ldaLearn(X,y):
    # Inputs
    # X - a N x d matrix with each row corresponding to a training example
    # y - a N x 1 column vector indicating the labels for each training example
    #
    # Outputs
    # means - A d x k matrix containing learnt means for each of the k classes
    # covmat - A single d x d learnt covariance matrix 
    # IMPLEMENT THIS METHOD 
    
    dimension = X.shape[1]
    means = np.zeros((dimension, int(np.max(y))))
    covmat = np.zeros(dimension)
    for label in range (0,int(np.max(y))):
        c = np.where(y==(label+1))[0]
        train = X[c,:]
        means[:,label] = np.mean(train,axis = 0).T
    covmat  = np.cov(X.T)
           
    return means,covmat

def qdaLearn(X,y):
    # Inputs
    # X - a N x d matrix with each row corresponding to a training example
    # y - a N x 1 column vector indicating the labels for each training example
    #
    # Outputs
    # means - A d x k matrix containing learnt means for each of the k classes
    # covmats - A list of k d x d learnt covariance matrices for each of the k classes
    # IMPLEMENT THIS METHOD
    
    dimension = X.shape[1]
    means = np.zeros((dimension, int(np.max(y))));
    covmats = []
    for label in range (0, int(np.max(y))):
        index = np.where(y==(label+1))[0]
        train = X[index,:]
        means[:, label] = np.mean(train, axis=0).T
        covmats.append(np.cov(train.T))
    
    return means,covmats

def ldaTest(means,covmat,Xtest,ytest):
    # Inputs
    # means, covmat - parameters of the LDA model
    # Xtest - a N x d matrix with each row corresponding to a test example
    # ytest - a N x 1 column vector indicating the labels for each test example
    # Outputs
    # acc - A scalar accuracy value
    # ypred - N x 1 column vector indicating the predicted labels
    # IMPLEMENT THIS METHOD
    solution = np.zeros((Xtest.shape[0],means.shape[1]))
    for i in range(means.shape[1]):
        solution[:,i]=(1/(sqrt(2*pi)*(np.linalg.det(covmat)**0.5)))*np.exp(-1*np.sum((Xtest - means[:,i])*np.dot((Xtest - means[:,i]),np.linalg.inv(covmat)),1)/2)
    
    ypred = np.argmax(solution,1)
    ypred = ypred + 1
    ytest = ytest.reshape(ytest.size)
    acc = 100*np.mean((ypred == ytest).astype(float))
    return acc,ypred

def qdaTest(means,covmats,Xtest,ytest):
    # Inputs
    # means, covmats - parameters of the QDA model
    # Xtest - a N x d matrix with each row corresponding to a test example
    # ytest - a N x 1 column vector indicating the labels for each test example
    # Outputs
    # acc - A scalar accuracy value
    # ypred - N x 1 column vector indicating the predicted labels
    # IMPLEMENT THIS METHOD
    solution = np.zeros((Xtest.shape[0],means.shape[1]))
    for i in range(means.shape[1]):
        solution[:,i]=(1/(sqrt(2*pi)*(np.linalg.det(covmats[i])**0.5)))*np.exp(-1*np.sum((Xtest - means[:,i])*np.dot((Xtest - means[:,i]),np.linalg.inv(covmats[i])),1)/2)
    
    ypred = np.argmax(solution,1)
    ypred = ypred + 1
    ytest = ytest.reshape(ytest.size)
    acc = 100*np.mean((ypred == ytest).astype(float))
    return acc,ypred

def learnOLERegression(X,y):
    # Inputs:                                                         
    # X = N x d 
    # y = N x 1                                                               
    # Output: 
    # w = d x 1 
	 # IMPLEMENT THIS METHOD                                                   
    w = np.dot(np.dot(np.linalg.inv(np.dot(X.T,X)),X.T),y)	
    return w

def learnRidgeRegression(X,y,lambd):
    # Inputs:
    # X = N x d                                                               
    # y = N x 1 
    # lambd = ridge parameter (scalar)
    # Output:                                                                  
    # w = d x 1                                                                
    # IMPLEMENT THIS METHOD                                                   
    d = X.shape[1]
    I = np.eye(d)*lambd
    w =np.dot(np.dot(np.linalg.inv(I+np.dot(X.T,X)),X.T),y)
    
    return w

def testOLERegression(w,Xtest,ytest):
    # Inputs:
    # w = d x 1
    # Xtest = N x d
    # ytest = X x 1
    # Output:
    # mse
    # IMPLEMENT THIS METHOD
    N = Xtest.shape[0]
    mse = (1/N)*np.dot((ytest-np.dot(Xtest,w)).T,(ytest-np.dot(Xtest,w)))
    
    return mse

def regressionObjVal(w, X, y, lambd):

    # compute squared error (scalar) and gradient of squared error with respect
    # to w (vector) for the given data X and y and the regularization parameter
    # lambda                                                                  

    # IMPLEMENT THIS METHOD                                             
    w1 = np.array([])
    w1 = w.reshape(65,1)
       
    error = 0.5*(np.dot(((y-np.dot(X,w1)).T),(y-np.dot(X,w1))))+0.5*lambd*(np.dot(w1.T,w1))
   
    error_grad = ((((np.dot(w1.T, np.dot(X.T, X)))-(np.dot(y.T, X)))) + (lambd * w)).flatten()
    
    return error, error_grad

def mapNonLinear(x,p):
    # Inputs:                                                                  
    # x - a single column vector (N x 1)                                       
    # p - integer (>= 0)                                                       
    # Outputs:                                                                 
    # Xd - (N x (d+1)) 
    	
    # IMPLEMENT THIS METHOD
    x1 = x.reshape((x.shape[0], 1))
    one_1 = (x.shape[0], 1)
    Xd = np.ones(one_1)
    
    for degree in range(1,p+1):
        new = np.power(x1,degree)
        Xd = np.concatenate((Xd, new), axis=1)
    
    return Xd

# Main script

# Problem 1
# load the sample data                                                                 
if sys.version_info.major == 2:
    X,y,Xtest,ytest = pickle.load(open('sample.pickle','rb'))
else:
    X,y,Xtest,ytest = pickle.load(open('sample.pickle','rb'),encoding = 'latin1')

# LDA
means,covmat = ldaLearn(X,y)
ldaacc,ldares = ldaTest(means,covmat,Xtest,ytest)
print('LDA Accuracy = '+str(ldaacc))
# QDA
means,covmats = qdaLearn(X,y)
qdaacc,qdares = qdaTest(means,covmats,Xtest,ytest)
print('QDA Accuracy = '+str(qdaacc))

# plotting boundaries
x1 = np.linspace(-5,20,100)
x2 = np.linspace(-5,20,100)
xx1,xx2 = np.meshgrid(x1,x2)
xx = np.zeros((x1.shape[0]*x2.shape[0],2))
xx[:,0] = xx1.ravel()
xx[:,1] = xx2.ravel()

fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)

zacc,zldares = ldaTest(means,covmat,xx,np.zeros((xx.shape[0],1)))
plt.contourf(x1,x2,zldares.reshape((x1.shape[0],x2.shape[0])),alpha=0.3)
plt.scatter(Xtest[:,0],Xtest[:,1],c=ytest)
plt.title('LDA')

plt.subplot(1, 2, 2)

zacc,zqdares = qdaTest(means,covmats,xx,np.zeros((xx.shape[0],1)))
plt.contourf(x1,x2,zqdares.reshape((x1.shape[0],x2.shape[0])),alpha=0.3)
plt.scatter(Xtest[:,0],Xtest[:,1],c=ytest)
plt.title('QDA')

plt.show()

# Problem 2
if sys.version_info.major == 2:
    X,y,Xtest,ytest = pickle.load(open('diabetes.pickle','rb'))
else:
    X,y,Xtest,ytest = pickle.load(open('diabetes.pickle','rb'),encoding = 'latin1')

# add intercept
X_i = np.concatenate((np.ones((X.shape[0],1)), X), axis=1)
Xtest_i = np.concatenate((np.ones((Xtest.shape[0],1)), Xtest), axis=1)

w = learnOLERegression(X,y)
mle = testOLERegression(w,Xtest,ytest)
mle_train = testOLERegression(w,X,y)

w_i = learnOLERegression(X_i,y)
mle_i = testOLERegression(w_i,Xtest_i,ytest)
mle_train_i = testOLERegression(w_i,X_i,y)

print('MSE without intercept '+str(mle))
print('MSE with intercept '+str(mle_i))

print('MSE without intercept train '+str(mle_train))
print('MSE with intercept train'+str(mle_train_i))


# Problem 3
k = 101
lambdas = np.linspace(0, 1, num=k)
i = 0
mses3_train = np.zeros((k,1))
mses3 = np.zeros((k,1))
for lambd in lambdas:
    w_l = learnRidgeRegression(X_i,y,lambd)
    mses3_train[i] = testOLERegression(w_l,X_i,y)
    mses3[i] = testOLERegression(w_l,Xtest_i,ytest)
    i = i + 1
fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(lambdas,mses3_train)
plt.title('MSE for Train Data')
plt.subplot(1, 2, 2)
plt.plot(lambdas,mses3)
plt.title('MSE for Test Data')

plt.show()

print("MSE test"+str(mses3[6]))
print("MSE train"+str(mses3_train[6]))


# Problem 4
k = 101
lambdas = np.linspace(0, 1, num=k)
i = 0
mses4_train = np.zeros((k,1))
mses4 = np.zeros((k,1))
opts = {'maxiter' : 40}    # Preferred value.                                                
w_init = np.ones((X_i.shape[1],1))
for lambd in lambdas:
    args = (X_i, y, lambd)
    w_l = minimize(regressionObjVal, w_init, jac=True, args=args,method='CG', options=opts)
    w_l = np.transpose(np.array(w_l.x))
    w_l = np.reshape(w_l,[len(w_l),1])
    mses4_train[i] = testOLERegression(w_l,X_i,y)
    mses4[i] = testOLERegression(w_l,Xtest_i,ytest)
    i = i + 1
fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(lambdas,mses4_train)
plt.plot(lambdas,mses3_train)
plt.title('MSE for Train Data')
plt.legend(['Using scipy.minimize','Direct minimization'])

plt.subplot(1, 2, 2)
plt.plot(lambdas,mses4)
plt.plot(lambdas,mses3)
plt.title('MSE for Test Data')
plt.legend(['Using scipy.minimize','Direct minimization'])
plt.show()

print("MSE train"+str(mses4_train[6]))
print("MSE test"+str(mses4[6]))

# Problem 5
pmax = 7
lambda_opt = 0.06 # REPLACE THIS WITH lambda_opt estimated from Problem 3
mses5_train = np.zeros((pmax,2))
mses5 = np.zeros((pmax,2))
for p in range(pmax):
    Xd = mapNonLinear(X[:,2],p)
    Xdtest = mapNonLinear(Xtest[:,2],p)
    w_d1 = learnRidgeRegression(Xd,y,0)
    mses5_train[p,0] = testOLERegression(w_d1,Xd,y)
    mses5[p,0] = testOLERegression(w_d1,Xdtest,ytest)
    w_d2 = learnRidgeRegression(Xd,y,lambda_opt)
    mses5_train[p,1] = testOLERegression(w_d2,Xd,y)
    mses5[p,1] = testOLERegression(w_d2,Xdtest,ytest)

fig = plt.figure(figsize=[12,6])
plt.subplot(1, 2, 1)
plt.plot(range(pmax),mses5_train)
plt.title('MSE for Train Data')
plt.legend(('No Regularization','Regularization'))
plt.subplot(1, 2, 2)
plt.plot(range(pmax),mses5)
plt.title('MSE for Test Data')
plt.legend(('No Regularization','Regularization'))
plt.show()

print("MSE train no-regularization "+str(mses5_train[1][0]))
print("MSE test no-regularization "+str(mses5[1][0]))

print("MSE train regularization "+str(mses5_train[1][1]))
print("MSE test regularization "+str(mses5[1][1]))