import numpy as np
from scipy.optimize import minimize
from scipy.io import loadmat
from math import sqrt
import pickle
import time


def initializeWeights(n_in, n_out):
    """
    # initializeWeights return the random weights for Neural Network given the
    # number of node in the input layer and output layer

    # Input:
    # n_in: number of nodes of the input layer
    # n_out: number of nodes of the output layer
       
    # Output: 
    # W: matrix of random initial weights with size (n_out x (n_in + 1))"""

    epsilon = sqrt(6) / sqrt(n_in + n_out + 1)
    W = (np.random.rand(n_out, n_in + 1) * 2 * epsilon) - epsilon
    return W


def sigmoid(z):
    """# Notice that z can be a scalar, a vector or a matrix
    # return the sigmoid of input z"""

    return 1 / (1 + np.exp(-z))


def preprocess():
    """ Input:
     Although this function doesn't have any input, you are required to load
     the MNIST data set from file 'mnist_sample.mat'.

     Output:
     train_data: matrix of training set. Each row of train_data contains 
       feature vector of a image
     train_label: vector of label corresponding to each image in the training
       set
     validation_data: matrix of training set. Each row of validation_data 
       contains feature vector of a image
     validation_label: vector of label corresponding to each image in the 
       training set
     test_data: matrix of training set. Each row of test_data contains 
       feature vector of a image
     test_label: vector of label corresponding to each image in the testing
       set

     - feature selection"""


    mat = loadmat('mnist_all.mat')
    print("data loaded")
        # ------------Initialize preprocess arrays----------------------#
    train_preprocess = np.zeros(shape=(50000, 784))
    validation_preprocess = np.zeros(shape=(10000, 784))
    test_preprocess = np.zeros(shape=(10000, 784))
    train_label_preprocess = np.zeros(shape=(50000,))
    validation_label_preprocess = np.zeros(shape=(10000,))
    test_label_preprocess = np.zeros(shape=(10000,))
    # ------------Initialize flag variables----------------------#
    train_len = 0
    validation_len = 0
    test_len = 0
    train_label_len = 0
    validation_label_len = 0
    # ------------Start to split the data set into 6 arrays-----------#
    for key in mat:
        # -----------when the set is training set--------------------#
        if "train" in key:
            label = key[-1]  # record the corresponding label
            tup = mat.get(key)
            sap = range(tup.shape[0])
            tup_perm = np.random.permutation(sap)
            tup_len = len(tup)  # get the length of current training set
            tag_len = tup_len - 1000  # defines the number of examples which will be added into the training set

            # ---------------------adding data to training set-------------------------#
            train_preprocess[train_len:train_len + tag_len] = tup[tup_perm[1000:], :]
            train_len += tag_len

            train_label_preprocess[train_label_len:train_label_len + tag_len] = label
            train_label_len += tag_len

            # ---------------------adding data to validation set-------------------------#
            validation_preprocess[validation_len:validation_len + 1000] = tup[tup_perm[0:1000], :]
            validation_len += 1000

            validation_label_preprocess[validation_label_len:validation_label_len + 1000] = label
            validation_label_len += 1000

            # ---------------------adding data to test set-------------------------#
        elif "test" in key:
            label = key[-1]
            tup = mat.get(key)
            sap = range(tup.shape[0])
            tup_perm = np.random.permutation(sap)
            tup_len = len(tup)
            test_label_preprocess[test_len:test_len + tup_len] = label
            test_preprocess[test_len:test_len + tup_len] = tup[tup_perm]
            test_len += tup_len
            # ---------------------Shuffle,double and normalize-------------------------#
    train_size = range(train_preprocess.shape[0])
    train_perm = np.random.permutation(train_size)
    train_data = train_preprocess[train_perm]
    train_data = np.double(train_data)
    train_data = train_data / 255.0
    train_label = train_label_preprocess[train_perm]

    validation_size = range(validation_preprocess.shape[0])
    vali_perm = np.random.permutation(validation_size)
    validation_data = validation_preprocess[vali_perm]
    validation_data = np.double(validation_data)
    validation_data = validation_data / 255.0
    validation_label = validation_label_preprocess[vali_perm]
    
    test_size = range(test_preprocess.shape[0])
    test_perm = np.random.permutation(test_size)
    test_data = test_preprocess[test_perm]
    test_data = np.double(test_data)
    test_data = test_data / 255.0
    test_label = test_label_preprocess[test_perm]

    # Feature selection
    all_data = np.vstack([train_preprocess, validation_preprocess])
    data_chk = np.all(all_data == all_data[0,:], axis=0)
    null_data = []
    selected_feature_labels = []
    for index in range(len(data_chk)):
        if(data_chk[index]):
            null_data.append(index)
        if(~data_chk[index]):
            selected_feature_labels.append(index)
    #reduce dimension of input attributes
    train_data = np.delete(train_data, null_data, axis=1)
    #print("train_data",train_data.shape)
    validation_data = np.delete(validation_data, null_data, axis =1)
    #print("validation_data",validation_data.shape)
    test_data = np.delete(test_data, null_data, axis=1)
    #print("test_data",test_data.shape)
    
    # creating bias row for hidden layer
    bias_row =np.ones((np.size(train_data,0),1))
    # concatenate bias with z column-wise
    train_data=np.concatenate((train_data,bias_row),axis=1)
    #print("train_data",train_data.shape)
    # creating bias row for hidden layer
    bias_row =np.ones((np.size(test_data,0),1))
    # concatenate bias with z column-wise
    test_data=np.concatenate((test_data,bias_row),axis=1)
    #print("test_data",test_data.shape)
    # creating bias row for hidden layer
    bias_row =np.ones((np.size(validation_data,0),1))
    # concatenate bias with z column-wise
    validation_data=np.concatenate((validation_data,bias_row),axis=1)
    #print("validation_data",validation_data.shape)
    
    print('preprocess done')


    return train_data, train_label, validation_data, validation_label, test_data, test_label, selected_feature_labels

def nnObjFunction(params, *args):
    """% nnObjFunction computes the value of objective function (negative log 
    %   likelihood error function with regularization) given the parameters 
    %   of Neural Networks, thetraining data, their corresponding training 
    %   labels and lambda - regularization hyper-parameter.

    % Input:
    % params: vector of weights of 2 matrices w1 (weights of connections from
    %     input layer to hidden layer) and w2 (weights of connections from
    %     hidden layer to output layer) where all of the weights are contained
    %     in a single vector.
    % n_input: number of node in input layer (not include the bias node)
    % n_hidden: number of node in hidden layer (not include the bias node)
    % n_class: number of node in output layer (number of classes in
    %     classification problem
    % training_data: matrix of training data. Each row of this matrix
    %     represents the feature vector of a particular image
    % training_label: the vector of truth label of training images. Each entry
    %     in the vector represents the truth label of its corresponding image.
    % lambda: regularization hyper-parameter. This value is used for fixing the
    %     overfitting problem.
       
    % Output: 
    % obj_val: a scalar value representing value of error function
    % obj_grad: a SINGLE vector of gradient value of error function
    % NOTE: how to compute obj_grad
    % Use backpropagation algorithm to compute the gradient of error function
    % for each weights in weight matrices.

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % reshape 'params' vector into 2 matrices of weight w1 and w2
    % w1: matrix of weights of connections from input layer to hidden layers.
    %     w1(i, j) represents the weight of connection from unit j in input 
    %     layer to unit i in hidden layer.
    % w2: matrix of weights of connections from hidden layer to output layers.
    %     w2(i, j) represents the weight of connection from unit j in hidden 
    %     layer to unit i in output layer."""

    n_input, n_hidden, n_class, training_data, training_label, lambdaval = args

    w1 = params[0:n_hidden * (n_input + 1)].reshape((n_hidden, (n_input + 1)))
    w2 = params[(n_hidden * (n_input + 1)):].reshape((n_class, (n_hidden + 1)))
    obj_val = 0
    
    # Your code here
    
    #-----------------------Feed forward pass-----------------------------
    z,o_class = feedForward(training_data,w1,w2);
    
   
    #---------------- Calculate total error at output---------------------
    # total number of training samples
    N = training_data.shape[0]+1
    # true label at each output node
    Y = np.zeros((training_data.shape[0],n_class))
    for i in range(training_data.shape[0]):
       Y[i][training_label[i].astype(int)] = 1
        
    #error
    J = (-1/N)*(np.sum(np.sum(np.multiply(Y,np.log(o_class))+np.multiply(np.subtract(np.ones((Y.shape)),Y),np.log(np.subtract(np.ones((o_class.shape)),o_class))),axis=1),axis=0))
    
    #regularization factor
    obj_val = J + (lambdaval/(2*N))*(np.sum(np.sum(np.square(w1),axis=1),axis=0)+np.sum(np.sum(np.square(w2),axis=1),axis=0))
    
    #---------------calculate gradient--------------------------------
    # Calculate delta
    delta = o_class - Y
    
    #Calculate gradient for W2
    grad_w2 = np.dot(delta.transpose(),z)
    
    # regularization factor
    grad_w2 = (1/N)*(grad_w2+(lambdaval*w2))

    #delete bias row from W2 and z
    w2 = np.delete(w2,w2.shape[1]-1,axis=1)
    z = np.delete(z,z.shape[1]-1,axis=1)
    
    # Calculate gradient for w1
    grad_w1 = np.dot(np.multiply(np.multiply(np.subtract(np.ones((z.shape)),z),z),np.dot(delta,w2)).transpose(),training_data)
    
    #regularization factor
    grad_w1 = (1/N)*(grad_w1+(lambdaval*w1))
	
    #obj_grad = np.array([])

    obj_grad = np.concatenate((grad_w1.flatten(), grad_w2.flatten()),0)
    
   
    #Make sure you reshape the gradient matrices to a 1D array. for instance if your gradient matrices are grad_w1 and grad_w2
    #you would use code similar to the one below to create a flat array



    # Make sure you reshape the gradient matrices to a 1D array. for instance if your gradient matrices are grad_w1 and grad_w2
    # you would use code similar to the one below to create a flat array
    # obj_grad = np.concatenate((grad_w1.flatten(), grad_w2.flatten()),0)
    #obj_grad = np.array([])

    return (obj_val, obj_grad)


def nnPredict(w1, w2, data):
    """% nnPredict predicts the label of data given the parameter w1, w2 of Neural
    % Network.

    % Input:
    % w1: matrix of weights of connections from input layer to hidden layers.
    %     w1(i, j) represents the weight of connection from unit i in input 
    %     layer to unit j in hidden layer.
    % w2: matrix of weights of connections from hidden layer to output layers.
    %     w2(i, j) represents the weight of connection from unit i in input 
    %     layer to unit j in hidden layer.
    % data: matrix of data. Each row of this matrix represents the feature 
    %       vector of a particular image
       
    % Output: 
    % label: a column vector of predicted labels"""

    labels = np.array([])
    # Your code here
    sig_dotp_hidden,sig_dotp_output=feedForward(data,w1,w2);
    labels = sig_dotp_output.argmax(axis=1)
    return labels


def feedForward (data,w1,w2):
    # calculate dot product at hidden layer
    dotp_hidden = np.inner(data,w1)
    # pass the output through sigmoid function
    sig_dotp_hidden = sigmoid(dotp_hidden);
    # create bias row for hidden layer
    bias = np.ones((sig_dotp_hidden.shape[0],1))
    # add bias row to z
    sig_dotp_hidden = np.concatenate((sig_dotp_hidden,bias),axis=1)
    # calculate dot product at output layer
    dotp_output = np.inner(sig_dotp_hidden,w2)
    # pass the output through sigmoid function
    sig_dotp_output = sigmoid(dotp_output);
    return (sig_dotp_hidden,sig_dotp_output)

#lambdaval = 0.0
i = 7
while(i<8):
    t0 = time.time()

    train_data, train_label, validation_data, validation_label, test_data, test_label, feature_labels = preprocess()
    #  Train Neural Network
        
    # set the number of nodes in input unit (not including bias unit)
    n_input = train_data.shape[1]-1
        
    # set the number of nodes in hidden unit (not including bias unit)
    n_hidden = 50
    i = i  + 5
    print("H = ",n_hidden)
    # set the number of nodes in output unit
    n_class = 10
    # initialize the weights into some random matrices
    initial_w1 = initializeWeights(n_input, n_hidden)
    initial_w2 = initializeWeights(n_hidden, n_class)
    
    # unroll 2 weight matrices into single column vector
    initialWeights = np.concatenate((initial_w1.flatten(), initial_w2.flatten()), 0)
    
    # set the regularization hyper-parameter
    lambdaval = 35
    #i = i + 5
    print("lambda",lambdaval)
    args = (n_input, n_hidden, n_class, train_data, train_label, lambdaval)
    # Train Neural Network using fmin_cg or minimize from scipy,optimize module. Check documentation for a working example
    opts = {'maxiter': 50}  # Preferred value.
    
    nn_params = minimize(nnObjFunction, initialWeights, jac=True, args=args, method='CG', options=opts)
    
    # In Case you want to use fmin_cg, you may have to split the nnObjectFunction to two functions nnObjFunctionVal
    # and nnObjGradient. Check documentation for this function before you proceed.
    # nn_params, cost = fmin_cg(nnObjFunctionVal, initialWeights, nnObjGradient,args = args, maxiter = 50)
    print("nn_params", nn_params.x.shape)
    
    # Reshape nnParams from 1D vector into w1 and w2 matrices
    w1 = nn_params.x[0:n_hidden * (n_input + 1)].reshape((n_hidden, (n_input + 1)))
    w2 = nn_params.x[(n_hidden * (n_input + 1)):].reshape((n_class, (n_hidden + 1)))
    
    # Test the computed parameters
    
    predicted_label = nnPredict(w1, w2, train_data)
    
    # find the accuracy on Training Dataset
    
    print('\n Training set Accuracy:' + str(100 * np.mean((predicted_label == train_label).astype(float))) + '%')
        
    predicted_label = nnPredict(w1, w2, validation_data)
    
    # find the accuracy on Validation Dataset
    
    print('\n Validation set Accuracy:' + str(100 * np.mean((predicted_label == validation_label).astype(float))) + '%')
    
    predicted_label = nnPredict(w1, w2, test_data)
    
    # find the accuracy on Validation Dataset
    
    print('\n Test set Accuracy:' + str(100 * np.mean((predicted_label == test_label).astype(float))) + '%')
    t1 = time.time()
    
    total = t1-t0
    print(total)
    
    obj = [feature_labels, n_hidden, w1, w2, lambdaval]
    # selected_features is a list of feature indices that you use after removing unwanted features in feature selection step
    pickle.dump(obj, open('params.pickle', 'wb'))
       
