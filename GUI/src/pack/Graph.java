package pack;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

import java.awt.Color;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.JCheckBox;
import javax.swing.Timer;

public class Graph extends JPanel implements ChangeListener, ItemListener {

	private JSlider GraphIntervalSlider;
	private GUI gui;
	private JLabel sliderLabel;
	private JPanel sliderPanel;
	private JCheckBox xCheckBox;
	private JCheckBox yCheckBox;
	private JCheckBox zCheckBox;

	// accelerometer
	private int ox = 5;
	private int oy = 250;

	// ayroscope
	private int ox2 = 5;
	private int oy2 = 550;

	// z graph
	private int ox3 = 5;
	private int oy3 = 600;

	private float[] gx = new float[500];
	private float[] gy = new float[500];
	private float[] gz = new float[500];
	private float[] ax = new float[500];
	private float[] ay = new float[500];
	private float[] az = new float[500];
	private double[] time = new double[500];

	private int i = 1;
	private boolean t = false;
	private boolean clear = false;
	private boolean xline = true;
	private boolean yline = true;
	private boolean zline = true;
	private int chance;
	private double scale;

	public void paint(Graphics g) {

		for (int j = 0; j < scale; j++) {
			time[j] = j * (500 / scale);

		}

		gx[0] = 0;
		gy[0] = 0;
		gz[0] = 0;
		ax[0] = 0;
		ay[0] = 0;
		az[0] = 0;

		super.paintComponents(g);

		// Blue box with X, Y, Z, Counter
		g.fillRect(0, 50, this.getWidth(), this.getHeight() - 50);

		g.setColor(Color.BLUE);
		g.fillRect(50, 375, 250, 25); // blue box
		g.setColor(Color.WHITE);
		g.drawString("X :" + ax[i], 60, 390);
		g.drawString("Y :" + ay[i], 110, 390);
		g.drawString("Z :" + az[i], 160, 390);
		g.drawString("Counter :" + i, 210, 390);

		g.setColor(Color.BLUE);
		g.fillRect(50, 675, 250, 25); // blue box
		g.setColor(Color.WHITE);
		g.drawString("X :" + (int)gx[i], 60, 690);
		g.drawString("Y :" + (int)gy[i], 110, 690);
		g.drawString("Z :" + (int)gz[i], 160, 690);
		g.drawString("Counter :" + i, 210, 690);

		// Drawing the lines

		for (int j = 1; j < i; j++) {

			// Accelerometer
			g.setColor(Color.RED);
			g.drawLine(ox, oy, (int) time[0], oy - (int) ax[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy - (int) ax[j - 1], (int) time[j],
					oy - (int) ax[j]); // horizontal line

			g.setColor(Color.GREEN);
			g.drawLine(ox, oy, (int) time[0], oy - (int) ay[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy - (int) ay[j - 1], (int) time[j],
					oy - (int) ay[j]); // horizontal line

			g.setColor(Color.BLUE);
			g.drawLine(ox, oy, (int) time[0], oy - (int) az[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy - (int) az[j - 1], (int) time[j],
					oy - (int) az[j]); // horizontal line

			// Gyroscope
			g.setColor(Color.RED);
			g.drawLine(ox2, oy2, (int) time[0], oy2 - (int) gx[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy2 - (int) gx[j - 1], (int) time[j],
					oy2 - (int) gx[j]); // horizontal line

			g.setColor(Color.GREEN);
			g.drawLine(ox2, oy2, (int) time[0], oy2 - (int) gy[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy2 - (int) gy[j - 1], (int) time[j],
					oy2 - (int) gy[j]); // horizontal line

			g.setColor(Color.BLUE);
			g.drawLine(ox2, oy2, (int) time[0], oy2 - (int) gz[0]); // horizontal
																	// line
			g.drawLine((int) time[j - 1], oy2 - (int) gz[j - 1], (int) time[j],
					oy2 - (int) gz[j]); // horizontal line

		}
		// Clear screen if clear is set to true
		if (clear == true) {
			// g.setColor(Color.BLACK);
			// g.fillRect(0, 0, this.getWidth(), this.getHeight());
			for (int j = 0; j < i; j++) {
				gx[j] = 0;
				gy[j] = 0;
				gz[j] = 0;
				ax[j] = 0;
				ay[j] = 0;
				az[j] = 0;
			}

			clear = false;
		}

		// Graph Labels
		g.setColor(Color.WHITE);
		g.drawString("Accelerometer Data", 220, 140);
		g.drawString("Gyroscope Data", 220, 440);

		// Graph White Line

		// accelerometer
		g.setColor(Color.WHITE);
		g.fillRect(0, 150, 5, 200); // vertical line
		g.fillRect(ox, oy - 2, 505, 5); // horizontal line

		// ayroscope
		g.fillRect(0, 450, 5, 180); // vertical line
		g.fillRect(ox2, oy2 - 2, 505, 5); // horizontal line

		// Fill Graphs Accelerometer

		g.setColor(Color.WHITE);

		g.drawString("X", 570, oy - 120);
		g.drawString("Y", 620, oy - 120);
		g.drawString("Z", 670, oy - 120);

		g.setColor(Color.BLACK);
		g.fillRect(550, oy - 110, 150, 220); // black box

		g.setColor(Color.RED);
		if (ax[i] > 0)
			g.fillRect(550, oy - (int) ax[i], 50, (int) ax[i]); // vertical line
		else
			g.fillRect(550, oy, 50, (int) -ax[i]);

		g.setColor(Color.GREEN);
		if (ay[i] > 0)
			g.fillRect(600, oy - (int) ay[i], 50, (int) ay[i]); // vertical line
		else
			g.fillRect(600, oy, 50, (int) -ay[i]);

		g.setColor(Color.BLUE);
		if (az[i] > 0)
			g.fillRect(650, oy - (int) az[i], 50, (int) az[i]); // vertical line
		else
			g.fillRect(650, oy, 50, (int) -az[i]);

		g.setColor(Color.WHITE);

		g.fillRect(550, oy, 150, 5); // horizontal line
		g.fillRect(550, oy - 100, 5, 200); // vertical line
		g.fillRect(600, oy - 100, 5, 200); // vertical line
		g.fillRect(650, oy - 100, 5, 200); // vertical line
		g.fillRect(700, oy - 100, 5, 200); // vertical line

		// Fill Graphs Gyroscope

		g.setColor(Color.WHITE);

		g.drawString("X", 570, oy2 - 110);
		g.drawString("Y", 620, oy2 - 110);
		g.drawString("Z", 670, oy2 - 110);

		g.setColor(Color.BLACK);
		g.fillRect(550, oy2 - 110, 150, 220); // black box

		g.setColor(Color.RED);
		if (gx[i] > 0)
			g.fillRect(550, oy2 - (int) gx[i], 50, (int) gx[i]); // vertical
																	// line
		else
			g.fillRect(550, oy2, 50, (int) -gx[i]);

		g.setColor(Color.GREEN);
		if (gy[i] > 0)
			g.fillRect(600, oy2 - (int) gy[i], 50, (int) gy[i]); // vertical
																	// line
		else
			g.fillRect(600, oy2, 50, (int) -gy[i]);

		g.setColor(Color.BLUE);
		if (gz[i] > 0)
			g.fillRect(650, oy2 - (int) gz[i], 50, (int) gz[i]); // vertical
																	// line
		else
			g.fillRect(650, oy2, 50, (int) -gz[i]);

		g.setColor(Color.WHITE);

		g.fillRect(550, oy2, 150, 5); // horizontal line
		g.fillRect(550, oy2 - 90, 5, 180); // vertical line
		g.fillRect(600, oy2 - 90, 5, 180); // vertical line
		g.fillRect(650, oy2 - 90, 5, 180); // vertical line
		g.fillRect(700, oy2 - 90, 5, 180); // vertical line

		// Happens at initial load up
		if (t == false) {
			this.setLayout(new FlowLayout(FlowLayout.LEFT));

			this.sliderPanel = new JPanel();
			// this.sliderPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
			// this.sliderPanel.setSize(200,200);
			// this.sliderPanel.setLocation(1000, 0);
			this.sliderPanel.setBackground(Color.BLACK);
			this.sliderLabel = new JLabel("Scale: " + scale);
			this.sliderLabel.setText("Scale : 500");
			this.sliderLabel.setForeground(Color.WHITE);
			this.GraphIntervalSlider = new JSlider(JSlider.HORIZONTAL, 50, 500,
					500);
			this.GraphIntervalSlider.addChangeListener(this);
			Dimension d = GraphIntervalSlider.getPreferredSize();
			this.GraphIntervalSlider.setPreferredSize(new Dimension(
					d.width + 150, d.height + 50));
			this.GraphIntervalSlider.setMajorTickSpacing(50);
			this.GraphIntervalSlider.setMinorTickSpacing(25);
			this.GraphIntervalSlider.setPaintTicks(true);
			this.GraphIntervalSlider.setPaintLabels(true);
			this.GraphIntervalSlider.setBackground(Color.BLACK);
			this.GraphIntervalSlider.setForeground(Color.WHITE);

			this.xCheckBox = new JCheckBox("X");
			this.xCheckBox.setForeground(Color.WHITE);
			this.xCheckBox.setBackground(Color.BLACK);
			this.xCheckBox.addItemListener(this);
			this.xCheckBox.setSelected(true);

			this.yCheckBox = new JCheckBox("Y");
			this.yCheckBox.setForeground(Color.WHITE);
			this.yCheckBox.setBackground(Color.BLACK);
			this.yCheckBox.addItemListener(this);
			this.yCheckBox.setSelected(true);

			this.zCheckBox = new JCheckBox("Z");
			this.zCheckBox.setForeground(Color.WHITE);
			this.zCheckBox.setBackground(Color.BLACK);
			this.zCheckBox.addItemListener(this);
			this.zCheckBox.setSelected(true);

			this.sliderPanel.add(xCheckBox);
			this.sliderPanel.add(yCheckBox);
			this.sliderPanel.add(zCheckBox);
			this.sliderPanel.add(GraphIntervalSlider);
			this.sliderPanel.add(sliderLabel);

			this.GraphIntervalSlider.setValue(500);

			this.add(sliderPanel);
			this.validate();
			this.repaint();

			scale = 500;

			this.setSize(1500, 800);
			init();
		}

	}

	private ChangeListener ChangeListener() {
		// TODO Auto-generated method stub
		return null;
	}

	public void init() {
		t = true;
		this.validate();
		this.repaint();
		Timer tmr = new Timer(1, new ActionListener() {
			public void actionPerformed(ActionEvent e) {

				repaint();
				i += 1;
				scale = GraphIntervalSlider.getValue();

				if (i >= scale) {
					i = 1;
					clear = true;
				}

				ax[i] = GUI.accl_x * 50;
				ay[i] = GUI.accl_y * 50;
				az[i] = GUI.accl_z * 50;
				
				gy[i] = GUI.gyro_x / 30;
				gx[i] = GUI.gyro_y / 30;
				gz[i] = GUI.gyro_z / 30;

				// If X, Y, Z check boxes are not checked
				if (xline == false) {
					ax[i] = 0;
					gx[i] = 0;
				}
				if (yline == false) {
					ay[i] = 0;
					gy[i] = 0;
				}
				if (zline == false) {
					az[i] = 0;
					gz[i] = 0;
				}

			}
		});
		tmr.start();

	}

	public void stateChanged(ChangeEvent e) {

		JSlider source = (JSlider) e.getSource();

		this.sliderLabel.setText("Scale : " + (int) scale);
		i = 1;
		clear = true;
	}

	public void itemStateChanged(ItemEvent e) {

		Object source = e.getItemSelectable();
		if (source == xCheckBox && xCheckBox.isSelected() == false)
			xline = false;
		if (source == xCheckBox && xCheckBox.isSelected() == true)
			xline = true;
		if (source == yCheckBox && yCheckBox.isSelected() == false)
			yline = false;
		if (source == yCheckBox && yCheckBox.isSelected() == true)
			yline = true;
		if (source == zCheckBox && zCheckBox.isSelected() == false)
			zline = false;
		if (source == zCheckBox && zCheckBox.isSelected() == true)
			zline = true;

	}
}