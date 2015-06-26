package pack;


import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenuBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;




public class MainFrame extends JFrame implements ActionListener {
	private JTextArea consoleArea;
	private JTextField commandField;
	
	public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();
		
		if(action == "Go") {
			consoleArea.append(commandField.getText() + "\n"); //Display text on commandField to Console
	//		commandField.setText("");
		//	System.out.println("Connected");
		}
		
	}
	
	public MainFrame(String title) {
		super(title);
		// Set layout manager
		setLayout(new BorderLayout());
		
		
	
		
		// Create Swing component
		JLabel comLabel = new JLabel("Command:");
		JLabel consoleLabel = new JLabel("Console:");
		
		
		this.commandField = new JTextField(24);
		this.consoleArea= new JTextArea(10,30);
		JScrollPane scrollPane = new JScrollPane(consoleArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		final JButton goButton = new JButton("Go");
		
		
		
		

		
		//Layout
		
		setLayout(new GridBagLayout());
		GridBagConstraints gc = new GridBagConstraints();
		gc.anchor = GridBagConstraints.LINE_START; //Line up labels column
		
		//First Row (Command Label)
		//gc.weightx = 0.5;
		//gc.weighty = 0.5;
		gc.gridx = 0;
		gc.gridy = 0;
		add(comLabel, gc);

		//Second Row (Command Text Field and Go button)
		gc.gridx = 0;
		gc.gridy = 1;
		add(commandField, gc);
		gc.anchor = GridBagConstraints.LINE_END;
		add(goButton, gc);
		gc.anchor = GridBagConstraints.LINE_START; 
		
		//Third Row (Console Label)
		gc.gridx = 0;
		gc.gridy = 2;
		add(consoleLabel, gc);
		
		//Fourth Row (Console TextArea)
		gc.gridx = 0;
		gc.gridy = 3;
		add(scrollPane,gc);
		
		//Add behavior
		
		
		

		goButton.addActionListener(this);
		goButton.setActionCommand("Go");
		
	
	/*
		goButton.addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent arg0) {
		
				consoleArea.append(commandField.getText() + "\n"); //Display text on commandField to Console
				commandField.setText("");
				
			} 
			
		});
	*/	


			
	}
	
	

}
