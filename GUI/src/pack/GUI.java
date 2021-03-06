package pack;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.text.DefaultCaret;

/**
 *	Responsible for creating all graphical related stuff i.e. commandline, console, buttons.. 
 *	Communicates directly with SerialCommunication to transmit and receive data 
 *	between the microcontroller via USART.
 * @author Aaron, Rashaad, Tapan
 *
 */
public class GUI extends JFrame implements ActionListener, KeyListener {

	private JTextField BaudRateText;
	private JComboBox portsCombo;
	private JTextField commandlineText;
	private JTextArea consoleArea;
	private String[] values = new String[5];
	private JButton sendButton;
	private SerialCommunication rs232;

	/**
	 * Create and show the GUI. Also instantiate the SerialCommunication and have
	 * it listening for responses to commands.
	 * @param name
	 */
	public GUI(String name) {
		super(name);

		/* Setup Communication Stuff */
		this.rs232 = new SerialCommunication(this);
		HashSet<String> availablePorts = this.rs232.getPorts();
		String[] ports = availablePorts.toArray(new String[availablePorts.size()]);
		/* End Comm Setup */

		this.pack();
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Read settings file and set each line to corresponding string value
		// index

		List<String> length = new ArrayList<String>();
		String s;

		try {
			BufferedReader br = new BufferedReader(new FileReader("settings"));
			while ((s = br.readLine()) != null) {
				length.add(s);

			}
			for (int i = 0; i < length.size(); i++) {
				values[i] = length.get(i);
			}

		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		} catch (IOException e) {
			System.out.println("Exception: " + e);
		}

		// Create a menu bar
		JMenuBar menubar = new JMenuBar();
		this.setJMenuBar(menubar);

		// Create file and exit menu items
		JMenu file = new JMenu("File");
		menubar.add(file);
		JMenuItem exit = new JMenuItem("Exit");
		exit.setActionCommand("exit");
		file.add(exit);
		exit.addActionListener(this);
		
		// set the frame dimensions
		this.setSize(700, 700);

		JPanel main = new JPanel();
		main.setLayout(new BoxLayout(main, BoxLayout.PAGE_AXIS));
		main.setBorder(BorderFactory.createTitledBorder("RS-232 Serial Communication"));

		// create a settings panel
		JPanel set = new JPanel();
		set.setLayout(new FlowLayout(FlowLayout.LEFT));
		set.setBorder(BorderFactory.createTitledBorder("Settings"));
		JLabel baudrateLabel = new JLabel("Set Baud Rate");
		this.BaudRateText = new JTextField(10);
		BaudRateText.setText(values[1]);

		// create the connect panel
		JPanel con = new JPanel();
		con.setLayout(new FlowLayout(FlowLayout.LEFT));
		con.setBorder(BorderFactory.createTitledBorder("Connection"));
		JLabel portLabel = new JLabel("Select Port:");
		JButton connect = new JButton("Connect");
		JButton disconnect = new JButton("Disconnect");
		JButton saveButton = new JButton("Save");
		
		disconnect.setActionCommand("disconnect");
		disconnect.addActionListener(this);
		
		// set the action to connect
		connect.setActionCommand("connect");
		connect.addActionListener(this); // enable listener

		// set the action to save
		saveButton.setActionCommand("save");
		saveButton.addActionListener(this); // enable listener

		// create commandline
		JPanel cmd = new JPanel();
		cmd.setLayout(new FlowLayout(FlowLayout.LEFT));
		cmd.setBorder(BorderFactory.createTitledBorder("Command"));
		JLabel cmdLabel = new JLabel("Command:");
		this.commandlineText = new JTextField(35);
		this.sendButton = new JButton("Send");
		this.sendButton.setActionCommand("send");
		this.sendButton.addActionListener(this);
		JButton clearBtn = new JButton("Clear");
		clearBtn.setActionCommand("Clear");
		clearBtn.addActionListener(this);
		
		cmd.add(cmdLabel);
		cmd.add(this.commandlineText);
		cmd.add(this.sendButton);
		cmd.add(clearBtn);

		// create the console
		this.consoleArea = new JTextArea(15, 54);
		this.consoleArea.setLineWrap(true);
		consoleArea.setBackground(Color.BLACK);
		consoleArea.setForeground(Color.GREEN);
		consoleArea.setSize(new Dimension(300, 300));
		JScrollPane scrollPane = new JScrollPane(consoleArea,JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		DefaultCaret caret=(DefaultCaret) consoleArea.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
		consoleArea.setBorder(BorderFactory.createMatteBorder(1, 1, 1, 1,Color.black));
		consoleArea.setVisible(true);

		JPanel consolePanel = new JPanel();
		consolePanel.setLayout(new FlowLayout(FlowLayout.LEFT));
		consolePanel.setBorder(BorderFactory.createTitledBorder("Console Input/Output"));

		this.portsCombo = new JComboBox(ports);
		// portsCombo.setSelectedIndex(Integer.parseInt(values[0]));

		set.add(baudrateLabel);
		set.add(BaudRateText);

		con.add(portLabel);
		con.add(portsCombo);
		con.add(connect);
		con.add(disconnect);
		con.add(saveButton);

		consolePanel.add(scrollPane);

		main.add(set);
		main.add(con);
		main.add(cmd);
		main.add(consolePanel);

		this.add(main);
		this.setVisible(true);

		// Add Key Listern for commandline Text
		this.commandlineText.addKeyListener(this);

	}
	
	/**
	 * Append Strings to the console showing a log of all the 
	 * commands that were typed (like a terminal)
	 */
	public void appendToConsole() {
		
		String command = this.commandlineText.getText();
		this.consoleArea.append("<command>: " + command + "\n");
		try {
			this.rs232.write(command);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Sending command: " + command + "\n");
		this.commandlineText.setText("");
	}
	
	public void appendToConsole(String s) {
		this.consoleArea.append(s);
	}
	
	private void clearConsole() {
		this.consoleArea.setText("");
	}
	
	/**
	 * Responses from the SerialReader are directed here upon 
	 * read completion.
	 * @param returned
	 */
	public void handleResponse(String returned) {
		if(returned.equalsIgnoreCase("ACK")) {
			this.consoleArea.append(">>ACKNOWLEDGE received..\n");
			this.consoleArea.append(">>Ready to communicate\n");
		}else {
			this.consoleArea.append("<response>" + returned + "\n");
		}
	}

	/**
	 * Listens for ActionEvent issued from the GUI
	 */
	public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();

		if (action == "connect") {
			try {
				String port = this.portsCombo.getSelectedItem().toString();
				this.rs232.connect(port);
				if (this.rs232.isConnected()) {
					this.consoleArea.append("..Attempting to connect on " + port + "\n");
					Thread.sleep(700);
					this.rs232.write("HELO root root"); //ask for Acknowledge using username/password
				}

			} catch (Exception e1) {
				e1.printStackTrace();
			}
		}else if (action == "send") {
			
			this.appendToConsole();
		
		}else if (action == "save") {
			values[0] = "" + portsCombo.getSelectedIndex();
			values[1] = "" + BaudRateText.getText();
			try {
				BufferedWriter bw = new BufferedWriter(new FileWriter("settings"));
				bw.write(values[0] + "\n");
				bw.write(values[1]);
				bw.close();
			} catch (IOException s) {
				s.printStackTrace();
			}
		}else if(action.equalsIgnoreCase("clear")) {
			this.clearConsole();
		}else if(action.equalsIgnoreCase("disconnect")) {
			this.rs232.disconnect();
		}
	}

	public void keyPressed(KeyEvent e) {
		// TODO Auto-generated method stub
		int key = e.getKeyCode();
		if (key == KeyEvent.VK_ENTER) {
			//Toolkit.getDefaultToolkit().beep();
			
			if(this.commandlineText.getText().equalsIgnoreCase("clear")) {
				this.clearConsole();
				this.commandlineText.setText("");
			}else {
				this.appendToConsole();
				this.commandlineText.setText("");
			}
		}
	}

	public void keyReleased(KeyEvent arg0) {
		// TODO Auto-generated method stub
	}

	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new GUI("QuadCopter USART Communication");
	}

}
