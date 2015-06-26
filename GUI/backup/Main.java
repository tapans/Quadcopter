package pack;
import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;




public class Main extends JFrame implements ActionListener {

	private static DetailsPanel detailsPanel;
	
	public Main (String name){
		super(name);
		detailsPanel = new DetailsPanel(this);
		JFrame mainFrame = new MainFrame("Quadcopter"); 
		this.add(mainFrame);
		
		//Create the menu bar.
		JMenuBar menubar = new JMenuBar();
		this.setJMenuBar(menubar);
	
		JMenu file = new JMenu ("File");
		menubar.add(file);
		JMenuItem settings = new JMenuItem("Settings");
		file.add(settings);
		JMenuItem exit = new JMenuItem("Exit");
		file.add(exit);

	class settingsaction implements ActionListener{
		public void actionPerformed (ActionEvent e) {
			JFrame settingsframe = new JFrame("Settings");
			settingsframe.add(detailsPanel);
			
			
			settingsframe.setLocation(160,160); //Default starting coordinates of frame
			settingsframe.setSize(480,380);
			settingsframe.setVisible(true);
		
		}
	}
	settings.addActionListener(new settingsaction());
	

	class exitaction implements ActionListener{
		public void actionPerformed (ActionEvent e) {
			System.exit(0);
		}
	}
    exit.addActionListener (new exitaction());

    this.setLocation(150,150); //Default starting coordinates of frame
	this.setSize(500,400);
    this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); //Allows App to close when X button at the top is clicked
    this.setVisible(true);
}

	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	
	public static void main (String[] args) {
		new Main("Quadcopter");
	}
		
}



