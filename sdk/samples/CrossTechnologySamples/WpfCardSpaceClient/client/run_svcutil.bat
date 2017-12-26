@rem enable data binding to data returned by the web service
@rem also turn on async access (best practice when used from GUI)
svcutil http://localhost/servicemodelsamples/service.svc/mex /edb /config:app.config /async /n:"http://Microsoft.ServiceModel.Samples,Microsoft.ServiceModel.Samples" /out:generatedClient.cs
