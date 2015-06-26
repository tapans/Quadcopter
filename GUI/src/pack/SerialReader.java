package pack;

import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import java.io.IOException;
import java.io.InputStream;

/**
 * SerialReader is responsible for handling all Read related events 
 * fired from SerialCommunication
 * @author Administrator
 *
 */
public class SerialReader implements SerialPortEventListener {

	private InputStream in;
	private byte[] buffer = new byte[2048];
	private GUI gui;

	/**
	 * Initialize the SerialReader with the SerialPort InputStream and OutputStream.
	 * @param in
	 * @param gui
	 */
	public SerialReader(InputStream in, GUI gui) {
		this.in = in;
		this.gui = gui; //This is overkill, please fix asap
	}

	/**
	 * This method will get called each time the SerialCommunication 
	 * issues a 'notifyOnDataAvailable' i.e. serialPort.notifyOnDataAvailable(true);
	 */
	public void serialEvent(SerialPortEvent arg0) {
		int data;
		try {
			int len = 0;
			while ((data = in.read()) > -1) {
				if (data == '\n') {
					break;
				}
				buffer[len++] = (byte) data;
			}
			//Send the response to the GUI (may need to be fixed)
			this.gui.handleResponse(new String(buffer,0,len));
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}
	}
}