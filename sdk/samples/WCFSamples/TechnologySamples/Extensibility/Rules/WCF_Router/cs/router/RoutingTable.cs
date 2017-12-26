//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Dispatcher;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel.Serialization;
using System.Xml;

namespace Microsoft.ServiceModel.Samples
{    
    //Since certain variables of routingtable (possibleaddress) aren't assigned to or
    //used anywhere except in rules, this will show up as warning.  This is to disable those warnings
    #pragma warning disable 0169
    #pragma warning disable 0649

    public class RoutingTable
    {
        Random randomNumberGenerator;
        Message currentMessage;
        IList<EndpointAddress> possibleAddress;
        EndpointAddress selectedAddress;
        XmlNamespaceManager manager;
        RuleSet ruleSet;
        RuleEngine ruleEngine;

        public RoutingTable()
        {
            this.randomNumberGenerator = new Random();
            this.manager = new XPathMessageContext();

            this.ruleSet = GetRuleSetFromFile(ConfigurationManager.AppSettings["SelectDestinationRuleSetName"], ConfigurationManager.AppSettings["SelectDestinationRulesFile"]);
            this.ruleEngine = new RuleEngine(ruleSet, typeof(RoutingTable));
        }

        public EndpointAddress SelectDestination(Message message)
        {
            this.currentMessage = message;

            this.ruleEngine.Execute(this);
            return this.selectedAddress;
        }

        private RuleSet GetRuleSetFromFile(string ruleSetName, string ruleSetFileName)
        {
            XmlTextReader routingTableDataFileReader = new XmlTextReader(ruleSetFileName);
            RuleDefinitions ruleDefinitions = null;
            if (routingTableDataFileReader != null)
            {
                try
                {
                    WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();
                    ruleDefinitions = serializer.Deserialize(routingTableDataFileReader) as RuleDefinitions;
                }
                catch (Exception e)
                {
                    throw new ApplicationException("The specified file is not a valid rules file \"" + ruleSetFileName + "\"", e);
                }
                if (ruleDefinitions != null)
                {
                    try
                    {
                        RuleSet ruleSet = ruleDefinitions.RuleSets[ruleSetName];
                        return ruleSet;
                    }
                    catch (Exception e)
                    {
                        throw new ApplicationException("The specified rules file does not have the ruleset with name : " + ruleSetName, e);
                    }
                }
                else
                {
                    throw new ApplicationException("There are no rulesets inside rules file");
                }
            }
            else
            {
                throw new ApplicationException("Can't find a valid file at the specified location. " + ruleSetFileName);
            }
        }
    }
}
