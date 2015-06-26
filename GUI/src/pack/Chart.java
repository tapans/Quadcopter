package pack;

import java.util.Random;
import javax.swing.JFrame;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.time.Millisecond;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;

public class Chart {
    @SuppressWarnings("deprecation")
	static TimeSeries ax = new TimeSeries("accl X", Millisecond.class);
    @SuppressWarnings("deprecation")
	static TimeSeries ay = new TimeSeries("accl Y", Millisecond.class);
    @SuppressWarnings("deprecation")
	static TimeSeries az = new TimeSeries("accl Z", Millisecond.class);

    public Chart(GUI gui) {
    	gen myGen = new gen();
        new Thread(myGen).start();

        TimeSeriesCollection dataset = new TimeSeriesCollection(ax);
        JFreeChart chart = ChartFactory.createTimeSeriesChart(
            "GraphTest",
            "Time",
            "Value",
            dataset,
            true,
            true,
            false
        );
        
        final XYPlot plot = chart.getXYPlot();
        ValueAxis axis = plot.getDomainAxis();
        axis.setAutoRange(true);
        axis.setFixedAutoRange(5000);

        JFrame frame = new JFrame("GraphTest");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        ChartPanel label = new ChartPanel(chart);
        frame.getContentPane().add(label);
        //Suppose I add combo boxes and buttons here later

        frame.pack();
        frame.setVisible(true);
    }
    
    /*public static void main(String[] args) throws InterruptedException {
        
    }*/

    static class gen implements Runnable {
  
    	
        private Random randGen = new Random();

        public void run() {
            while(true) {
                //System.out.println(num);
            	ax.addOrUpdate(new Millisecond(), GUI.x);
                ay.addOrUpdate(new Millisecond(), GUI.y);
                az.addOrUpdate(new Millisecond(), GUI.z);
                //az.addOrUpdate(new Millisecond(), GUI.accl_z * 50);
                try {
                    Thread.sleep(2);
                } catch (InterruptedException ex) {
                    System.out.println(ex);
                }
            }
        }
    }

}