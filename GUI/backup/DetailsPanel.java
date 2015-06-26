package pack;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class DetailsPanel extends JPanel {
	
	private Main main;
	
	public DetailsPanel(Main main) {
		this.main = main;
		
		final String[] values;
		values = new String[5];
	  	List<String> length = new ArrayList<String>();
		String s;  
		
		try {
			BufferedReader br = new BufferedReader(new FileReader("settings"));
			while((s = br.readLine()) != null){  
		  		 length.add(s);
		  		 
		  	 }	
			 for (int i = 0; i < length.size(); i++) {
				values[i] = length.get(i);
			 }
			
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		}
	  	catch(IOException e) {
		      System.out.println("Exception: " + e);
		    }
	

 
		Dimension size = getPreferredSize();
		size.width = 250;
		setPreferredSize(size);


		
		setBorder(BorderFactory.createTitledBorder("Settings"));
		JLabel comLabel = new JLabel("Communcation:");
		JLabel baudrateLabel = new JLabel("Baud Rate:");
		
		String[] portStrings = { "Port1", "Port2", "Port3", "Port4", "Port5" };
		
		
		final JComboBox portList = new JComboBox(portStrings);
		portList.setSelectedIndex(Integer.parseInt(values[0])); //Set Port 1 as default
		
		
		final JTextField baudrateField = new JTextField(10);
		baudrateField.setText(values[1]);
		
		JButton save = new JButton("Save");
		
		setLayout(new GridBagLayout());
		GridBagConstraints gc = new GridBagConstraints();
		
		gc.anchor = GridBagConstraints.LINE_START; //Line up labels column
		
		//// First column ////
		
		gc.weightx = 0.5;
		gc.weighty = 0.5;
		gc.gridx = 0;
		gc.gridy = 0;
		add(comLabel, gc);
		
		gc.gridx = 0;
		gc.gridy = 1;
		add(baudrateLabel, gc);
		
		//// Second column
		
		gc.gridx = 1;
		gc.gridy = 0;
		add(portList, gc);
		
		
		gc.gridx = 1;
		gc.gridy = 1;
		add(baudrateField,gc);
		
		//// Save button third row
		gc.weighty = 10;
		gc.gridx = 0;
		gc.gridy = 2;
		add(save, gc);
		

					
	
		save.addActionListener(new ActionListener() {
			
			public final void actionPerformed(ActionEvent arg0) {
				values[0] = "" + portList.getSelectedIndex();
				values[1] = "" + baudrateField.getText();		
				
				try {
					BufferedWriter bw = new BufferedWriter(new FileWriter("settings"));
					bw.write(values[0] + "\n");
					bw.write(values[1]);
					bw.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
			} 
			
		});
		

		
	}
	
}
