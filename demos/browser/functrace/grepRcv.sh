#!/bin/bash

egrep -n '(didFail|phase|sendResponseIfNeeded|didReceiveResponse|forwardData|didReceiveData|didFinishLoading|\[dual\]|postRequest|validateCache|sendCacheContents)' reyou1_idle > you_grep
#egrep -n '(didFail|phase|sendResponseIfNeeded|didReceiveResponse|forwardData|didReceiveData|didFinishLoading|\[dual\]|postRequest|validateCache|sendCacheContents)' reimg_dual > reimg_dual_grep
