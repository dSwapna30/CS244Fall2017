%matplotlib inline

from scipy.signal import butter, lfilter

# Creates and returns Butterworth bandpass filter by given cutoff frequencies, sampling rate, and order
def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='bandpass')
    return b, a

# Applies Butterworth bandpass filter to given data and returns filtered data
def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

# Counts peaks of given dataset
def count_peaks(dataset):
    peak_num = 0
    peak_time = []
    peak_val = []
    for (num, data) in enumerate(dataset):
        if 1 < num < len(dataset):
            if dataset[num-1] < data  and data > dataset[num+1]:
                peak_time.append(num*0.02)
                peak_val.append(data)
                peak_num += 1
    return peak_time, peak_val, peak_num

if __name__ == "__main__":
    from numpy import genfromtxt, fft, pi, sqrt
    import matplotlib.pyplot as plt
    from scipy.signal import freqz
    data = genfromtxt('sample_no_header.csv',delimiter=',') # Be sure to remove the header of "sample.csv" beforehands

    # Sample rate and desired cutoff frequencies (in Hz).
    fs = 50.0
    lowcut = 1/6
    highcut = 1/3
    
    # Extract sampling time and IR data from CSV file
    time = [row[0] for row in data]
    IR = [row[1] for row in data]

    # Filter IR data
    Filtered_IR = butter_bandpass_filter(IR, lowcut, highcut, fs, order=4)
    
    # Find peak time, peak value and number of peaks in filtered data
    peak_time, peak_val, peak_num = count_peaks(Filtered_IR)
    
    for i, t in enumerate(peak_time):
        if i == 0:
            print(60/t)
        elif i < len(peak_time)-1:
            print(60/(peak_time[i+1]-t))
    
    # Calculate and display resipration rate
    print('Average respiration rate: ', peak_num/90*60)
    
    # Display filtered data with peaks (closer look up to small signals)
    plt.figure(1)
    plt.xlabel('time(second)')
    plt.plot(time, Filtered_IR, 'g', lw=1.3, label="filtered IR signal")
    plt.plot(peak_time, peak_val, 'o', label="peak")
    plt.title('Filtered IR signal (close look up)')
    plt.legend(loc='best')
    plt.xlim([30, 90])
    plt.ylim([-350, 350])
    
    # Display filtered data with peaks
    plt.figure(2)
    plt.xlabel('time(second)')
    plt.plot(time, Filtered_IR, 'g', lw=1.3, label="filtered IR signal")
    plt.plot(peak_time, peak_val, 'o', label="peak")
    plt.legend(loc='best')
    plt.title('Filtered IR signal')
    
    # Display bandpass filter Bode plot
    plt.figure(3)
    for order in [1, 2, 3, 4, 5, 6, 9]:
        b, a = butter_bandpass(lowcut, highcut, fs, order=order)
        w, h = freqz(b, a, worN=2000) # freqz: Calculates frequency response of a given filter
        plt.plot((fs * 0.5 / pi) * w, abs(h), label="order = %d" % order, lw=1.3)
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Gain')
    plt.grid(True)
    plt.legend(loc='best')
    plt.title('Bode plot')
    plt.xlim([0, 1])
    
    plt.figure(4)
    plt.plot(fft.fft(IR), 'r', lw=1.3)
    plt.xlim([0, 10])
    
    plt.show()