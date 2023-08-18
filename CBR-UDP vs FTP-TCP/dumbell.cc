#include <bits/stdc++.h>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include <cstdlib>
#include "ns3/flow-monitor-module.h"
#include <string.h>
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/gnuplot.h"
#include "ns3/ipv4-global-routing-helper.h"


typedef uint32_t uint;

using namespace ns3;  // this is our main namespace and in additional to that std namespace has been used whenever necessary

#define ERROR 0.0001  // error rate which is used in RateErrorModel

int main (int argc, char *argv[])
{

    // initializing the required variables 
    uint32_t maxBytes = 0;
	uint32_t portNo;
	uint32_t packetsize = 1024;
	uint32_t run_time = 1;
	uint32_t iters = 1;
	uint32_t offset=0;
	bool simultaneously = false;
    std::string protocol = "TcpHighSpeed";

    // since an IDE has not been used, the traditional command line is used for taking the inputs
	CommandLine cmd;
    cmd.AddValue ("packetsize", "size of the packet it sends", packetsize);
    cmd.AddValue ("protocol", "various protocols such as TcpHighSpeed, TcpVegas, TcpScalable", protocol);
    cmd.AddValue ("iters", " # of iterations ", iters);
    cmd.AddValue ("run_time", "total time at a step of 5", run_time);
    cmd.AddValue ("simultaneously", "boolean variable to control the concurrency of both agents", simultaneously);
    cmd.AddValue ("offset", "offset for different start times", offset);

    cmd.Parse (argc, argv);

    // making the protocol string into lowercase to avoid any errors occurred during the assigning of the protocol value
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);

    if (protocol.compare("tcpscalable") == 0)
    {
        // setDefault let's us assign the TypeID to the specified attribute which in this case is SocketType
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpScalable::GetTypeId()));
    }
    else if (protocol.compare("tcpvegas") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpVegas::GetTypeId()));
    }
    else if (protocol.compare("tcphighspeed") == 0)
    {
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHighSpeed::GetTypeId()));
    }
    else
    {
        // If nothing is specified let's use the TcpHighSpeed 
        Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpHighSpeed::GetTypeId()));
    }

	
    // gnuplot is used for plotting the comparision graph
    Gnuplot2dDataset udp_dataset;
	Gnuplot2dDataset tcp_dataset;
    Gnuplot2dDataset udpDelay_dataset;
	Gnuplot2dDataset tcpDelay_dataset;

	for(uint i=0;i<iters; ++i)
	{		
        // initializing the packet sizes of both tcp and udp
		uint32_t udpPacketSize= packetsize+100*i;  
		uint32_t tcpPacketSize= udpPacketSize;  

		// creating a node container for 6 nodes(H1, H2, H3, H4 and R1, R2)
		NodeContainer nc;
		nc.Create(6);

        // assigning a unique identifier to each interface in the topology and making each
        // interface a container 
		NodeContainer n0n2 = NodeContainer(nc.Get(0), nc.Get(2));
		NodeContainer n1n2 = NodeContainer(nc.Get(1), nc.Get(2));
		NodeContainer n2n3 = NodeContainer(nc.Get(2), nc.Get(3));
		NodeContainer n3n4 = NodeContainer(nc.Get(3), nc.Get(4));
		NodeContainer n3n5 = NodeContainer(nc.Get(3), nc.Get(5));


		// making the nodes capable of using the internet stack
		InternetStackHelper internet;
		internet.Install(nc);

        // defining the queue sizes for both host to router and router to host interfaces
		uint32_t queueSizeHR = (80000*20)/(udpPacketSize*8);
		uint32_t queueSizeRR = (30000*100)/(udpPacketSize*8);

        // storing the sizes in form of packets to use them later as attribute values
		std::string queueSizeHR2=std::to_string(queueSizeHR) + "p";
		std::string queueSizeRR2=std::to_string(queueSizeRR) + "p";


		// to create point to point links between nodes
		PointToPointHelper p2p;
		
		// creating the router to host links with defined attribute values
		p2p.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
		p2p.SetChannelAttribute("Delay", StringValue("20ms"));
	    p2p.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue(QueueSize(queueSizeHR2)));

        // net device containers for the links between hosts and routers
        // this adds a mac address and queue 
		NetDeviceContainer d0d2 = p2p.Install(n0n2);
		NetDeviceContainer d1d2 = p2p.Install(n1n2);
		NetDeviceContainer d3d4 = p2p.Install(n3n4);
		NetDeviceContainer d3d5 = p2p.Install(n3n5);

		// channel between the routers
		p2p.SetDeviceAttribute("DataRate", StringValue("30Mbps"));
		p2p.SetChannelAttribute("Delay", StringValue("100ms"));
	    p2p.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue(QueueSize(queueSizeRR2)));

        // net device container for the link between the routers
		NetDeviceContainer d2d3 = p2p.Install(n2n3);

		/* Error models are used to indicate that a packet should be considered to be errored, 
        according to the underlying error model.
        Ex: If ErrorRate is 0.1 and ErrorUnit set to “Packet”, in the long run, around 10% of the packets will be lost. */
		Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
	    em->SetAttribute("ErrorRate", DoubleValue(ERROR));

        // since H3 and H4 are used as destinations the error rate model is employed at them.
	    d3d4.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
	    d3d5.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));

		// assigning IP to the netdevice containers and making them as interfaces
		Ipv4AddressHelper ipv4;

        // SetBase takes network address, mask and base address as inputs
		ipv4.SetBase("192.168.0.0", "255.255.255.0");
		Ipv4InterfaceContainer i0i2 = ipv4.Assign(d0d2);
		
		ipv4.SetBase("192.168.1.0", "255.255.255.0");
		Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);

		ipv4.SetBase("192.168.3.0", "255.255.255.0");
		Ipv4InterfaceContainer i2i3 = ipv4.Assign(d2d3);

		ipv4.SetBase("192.168.4.0", "255.255.255.0");
		Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);

		ipv4.SetBase("192.168.5.0", "255.255.255.0");
		Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);

		
		// routuing tables 
		Ipv4GlobalRoutingHelper::PopulateRoutingTables();
		
		// routing tables of all the nodes in the container
		Ipv4GlobalRoutingHelper routing_table;
		Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("routing.routes", std::ios::out);
		routing_table.PrintRoutingTableAllAt(Seconds(2), routingStream);


        // assigning the port to UDP to measure the CBR traffic on UDP
		portNo = 9;  

        // on off helper is used to instantiate OnOffApplication which inturn is used to 
        // generate the traffic to a destination according to an OnOff pattern
		// making H3 as receiver
		OnOffHelper onoffapp("ns3::UdpSocketFactory", Address(InetSocketAddress(i3i4.GetAddress(1), portNo)));
	    onoffapp.SetAttribute("PacketSize", UintegerValue(udpPacketSize));

		
		// using the on off application on H1 as source for UDP traffic
		ApplicationContainer udp_source_traffic = onoffapp.Install(n0n2.Get(0));
		
		// varying the run time based on the concurrency
		if(simultaneously==false)
		{
			udp_source_traffic.Start(Seconds((0.0+(10*i))*run_time));
			udp_source_traffic.Stop(Seconds((5.0+(10*i))*run_time));			
		}
		else
		{
			udp_source_traffic.Start(Seconds((0.0+(10*i))*run_time));
			udp_source_traffic.Stop(Seconds((10.0+(10*i))*run_time));
		}

		// packet sink is used to consume traffic generated to an IP address and port
		PacketSinkHelper udp_sink("ns3::UdpSocketFactory",Address(InetSocketAddress(Ipv4Address::GetAny(), portNo)));

		// install the reciver on HOST-3
		ApplicationContainer udp_dest_traffic = udp_sink.Install(n3n4.Get(1));

		if(simultaneously==false)
		{
			udp_dest_traffic.Start(Seconds((0.0+(10*i))*run_time));
			udp_dest_traffic.Stop(Seconds((5.0+(10*i))*run_time));			
		}
		else
		{
			udp_dest_traffic.Start(Seconds((0.0+(10*i))*run_time));
			udp_dest_traffic.Stop(Seconds((10.0+(10*i))*run_time));			
		}



		// assigning the port to TCP to measure the FTP traffic on TCP
	    portNo = 26898;

        // BulkSendApplication is used to send large data 
	    BulkSendHelper source("ns3::TcpSocketFactory",InetSocketAddress(i3i5.GetAddress(1), portNo));
	    source.SetAttribute("MaxBytes", UintegerValue(maxBytes));  // By default zero means infinite data
		source.SetAttribute("SendSize", UintegerValue(tcpPacketSize));
	    ApplicationContainer tcp_source_traffic = source.Install(n1n2.Get(0));
	    
        // varying the run time based on the concurrency
	    if(simultaneously==false)
		{
			tcp_source_traffic.Start(Seconds((5.0+(10*i))*run_time));
	    	tcp_source_traffic.Stop(Seconds((10.0+(10*i))*run_time));
		}
		else
		{
			tcp_source_traffic.Start(Seconds((0.0+(10*i))*run_time));
	    	tcp_source_traffic.Stop(Seconds((10.0+(10*i))*run_time));	
		}

	    
	    // install the receiver on HOST-4
	    PacketSinkHelper tcp_sink("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(), portNo));
	    ApplicationContainer tcp_dest_traffic = tcp_sink.Install(n3n5.Get(1));

	    if(simultaneously==false)
		{
			tcp_dest_traffic.Start(Seconds((5.0+(10*i))*run_time));
	    	tcp_dest_traffic.Stop(Seconds((10.0+(10*i))*run_time));
		}
		else
		{
			tcp_dest_traffic.Start(Seconds((0.0+(10*i))*run_time+offset));
	    	tcp_dest_traffic.Stop(Seconds((10.0+(10*i))*run_time+offset));	
		}



		// flow monitor to store the performance data of the network
		Ptr<FlowMonitor> flowmon;
		FlowMonitorHelper flowmonHelper;
		flowmon = flowmonHelper.InstallAll();
		if(!simultaneously)
		Simulator::Stop(Seconds((10+(10*i))*run_time));
		else
		Simulator::Stop(Seconds((10+(10*i))*run_time+offset));
		Simulator::Run();
		flowmon->CheckForLostPackets();

        // classifies packets by looking at their headers
        // a unique tuple is created and a unique identifier is also assigned
		Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

		// storing the results into a map
		std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats();
		
		double udp_throughput;
		double tcp_throughput;
		double udp_delay;
		double tcp_delay;
		
		for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = stats.begin(); it != stats.end(); ++it) 
		{
			Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (it->first);

			if(t.sourceAddress == "192.168.0.1" || t.sourceAddress == "192.168.1.1")
			{
				std::cout<<"Flow: "<<it->first<<"\n";
				std::cout<<"Source: "<<t.sourceAddress<<"\n";
				std::cout<<"Destination: "<<t.destinationAddress<<"\n";
			}

            // since different sources are used for different communications,
            // it can be sorted out using the source address
			if(t.sourceAddress == "192.168.0.1") 	// for UDP
			{
                // calculating the throughput and delay based on the packets
				udp_throughput = (double)it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstRxPacket.GetSeconds()) / (double)1000;
				udp_delay = (double)it->second.delaySum.GetSeconds()/(it->second.rxPackets) ;

				udp_dataset.Add(udpPacketSize,udp_throughput);
				udpDelay_dataset.Add(udpPacketSize,udp_delay);

				std::cout << "Total Delay(in seconds): " << it->second.delaySum.GetSeconds() << std::endl;
				std::cout << "Throughput: " << (double)it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstRxPacket.GetSeconds()) / (double)1000 << " Kbps" << std::endl;
				std::cout<<std::endl;

			} 
			else if(t.sourceAddress == "192.168.1.1")	// for TCP
			{
				tcp_throughput = (double)it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstRxPacket.GetSeconds()) / 1000;
				tcp_delay = (double)it->second.delaySum.GetSeconds()/(it->second.rxPackets);
				
				tcp_dataset.Add(tcpPacketSize,tcp_throughput);
				tcpDelay_dataset.Add(tcpPacketSize,tcp_delay);

				std::cout << "Total Delay(in seconds): " << it->second.delaySum.GetSeconds() << std::endl;
				std::cout << "Throughput: " << (double)it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstRxPacket.GetSeconds()) / (double)1000 << " Kbps" << std::endl;
				std::cout<<std::endl;
			}
		}
		std::cout<<"Iteration: "<<i<<std::endl;
		std::cout<<std::endl;
		
		Simulator::Destroy();
	}

    // to take care of different cases
	std::string concurrency="Seperate";
	if(simultaneously&&offset==0)
		concurrency="Same_Start";
	else if(simultaneously&&offset!=0)
		concurrency="Different_Start";
	std::string output 					= protocol + "_throughput_" + concurrency;
	std::string graphics        		= output + ".png";
	std::string plotFileName            = output + ".plt";
	std::string plotTitle               = protocol + ": TCP vs UDP throughput";
	
	std::string output_delay 			= protocol + "_delay_" + concurrency;
	std::string graphics_delay        	= output_delay + ".png";
	std::string plotFileName_delay      = output_delay + ".plt";
	std::string plotTitle_delay         = protocol + ": TCP vs UDP delay";

	// making two different plots for delay and throughput
	Gnuplot plot(graphics);
	Gnuplot plot_delay(graphics_delay);
	
	plot.SetTitle(plotTitle);
	plot_delay.SetTitle(plotTitle_delay);

	// making the plot output as png
	plot.SetTerminal("png");
	plot_delay.SetTerminal("png");

	// setting the yrange for the plots
	plot.AppendExtra("set yrange [0:5000]");

	// Set the labels for each axis.
	plot.SetLegend("Packet Size(in Bytes)", "Throughput Values(in Kbps)");
	plot_delay.SetLegend("Packet Size(in Bytes)", "Delay(in s)");

	// inserting the title of the plot and styling the plot
	tcp_dataset.SetTitle(" FTP over TCP");
	tcp_dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	tcp_dataset.SetExtra("lw 2");
	udp_dataset.SetTitle(" CBR over UDP");
	udp_dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	udp_dataset.SetExtra("lw 2");
	
	tcpDelay_dataset.SetTitle("Delay FTP over TCP");
	tcpDelay_dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	tcpDelay_dataset.SetExtra("lw 2");
	udpDelay_dataset.SetTitle("Delay CBR over UDP");
	udpDelay_dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
	udpDelay_dataset.SetExtra("lw 2");


	// add the dataset to the plot.
	plot.AddDataset(tcp_dataset);
	plot.AddDataset(udp_dataset);
	
	plot_delay.AddDataset(udpDelay_dataset);
	plot_delay.AddDataset(tcpDelay_dataset);

	// open the plot file.
	std::ofstream plotFile(plotFileName.c_str());

	// write the plot file.
	plot.GenerateOutput(plotFile);

	// close the plot file.
	plotFile.close();

	std::ofstream plotFile_delay(plotFileName_delay.c_str());
	plot_delay.GenerateOutput(plotFile_delay);
	plotFile_delay.close();

	return 0;
}	