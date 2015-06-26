package pack;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import javax.swing.JTextArea;

import gnu.io.*;

public class SerialCommunication {

	private InputStream in;
	private OutputStream output;
	private Boolean connected;
	private GUI gui;
	private SerialPort serialPort;
	
	
	public SerialCommunication(GUI gui) {
		this.connected = false;
		this.gui = gui;
	}

	public void connect(String portName) throws Exception {
		CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);
		if (portIdentifier.isCurrentlyOwned()) {
			System.out.println("Error: Port is currently in use");
		}else {
			CommPort commPort = portIdentifier.open(this.getClass().getName(),2000);

			if(commPort instanceof SerialPort) {
				this.serialPort = (SerialPort) commPort;
				this.serialPort.setSerialPortParams(19200, SerialPort.DATABITS_8,SerialPort.STOPBITS_2, SerialPort.PARITY_NONE);
				this.serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);

				this.in = this.serialPort.getInputStream();
				this.output = this.serialPort.getOutputStream();

				this.connected = true;
				
				this.serialPort.addEventListener(new SerialReader(this.in, this.gui));
                this.serialPort.notifyOnDataAvailable(true);

			}else {
				System.out.println("Error: Only serial ports are handled by this example.");
			}
		}
	}
	
	public void disconnect() {
	    if (serialPort != null) {
	        try {
	            // close the i/o streams
	            this.output.close();
	            this.in.close();
	        } catch (IOException ex) {
	        	System.out.println(ex.getMessage());
	        }
	        this.serialPort.close();
	    }
	}
	
	/**
	 * Return true if connected to the Serial device
	 * @return
	 */
	public Boolean isConnected() {
		return this.connected;
	}

	/**
	 * Write a string to the USART. String is also null-terminated along with 
	 * end of line character.
	 * @param s
	 * @throws IOException
	 */
	public void write(String s) throws IOException {
		String serialMessage = s + "\0\n";
		this.output.write(serialMessage.getBytes());
	}

	/**
	 * @return A HashSet containing the CommPortIdentifier for all serial ports
	 * that are not currently being used.
	 */
	public static HashSet getAvailableSerialPorts() {
		HashSet h = new HashSet();
		Enumeration thePorts = CommPortIdentifier.getPortIdentifiers();
		while (thePorts.hasMoreElements()) {
			CommPortIdentifier com = (CommPortIdentifier) thePorts
					.nextElement();
			switch (com.getPortType()) {
			case CommPortIdentifier.PORT_SERIAL:
				try {
					CommPort thePort = com.open("CommUtil", 50);
					thePort.close();
					h.add(com);
				} catch (PortInUseException e) {
					System.out.println("Port, " + com.getName()
							+ ", is in use.");
				} catch (Exception e) {
					System.err.println("Failed to open port " + com.getName());
					e.printStackTrace();
				}
			}
		}
		return h;
	}

	/**
	 * Redundant and overkill, please use getAvailableSerialPorts and kill this method
	 * @return
	 */
	public static HashSet<String> getPorts() {
		HashSet portIdentifiers = SerialCommunication.getAvailableSerialPorts();
		Iterator it = portIdentifiers.iterator();

		HashSet<String> ports = new HashSet<String>();

		while (it.hasNext()) {
			ports.add(((CommPortIdentifier) it.next()).getName());
		}

		return ports;

	}

}
